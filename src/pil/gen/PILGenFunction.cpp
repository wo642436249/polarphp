//===--- PILGenFunction.cpp - Top-level lowering for functions ------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file defines the primary routines for creating and emitting
//  functions.
//
//===----------------------------------------------------------------------===//

#include "polarphp/pil/gen/PILGenFunction.h"
#include "polarphp/pil/gen/RValue.h"
#include "polarphp/pil/gen/PILGenFunctionBuilder.h"
#include "polarphp/pil/gen/Scope.h"
#include "polarphp/ast/ClangModuleLoader.h"
#include "polarphp/ast/FileUnit.h"
#include "polarphp/ast/Initializer.h"
#include "polarphp/ast/ParameterList.h"
#include "polarphp/ast/PropertyWrappers.h"
#include "polarphp/pil/lang/PILArgument.h"
#include "polarphp/pil/lang/PILProfiler.h"
#include "polarphp/pil/lang/PILUndef.h"
#include "polarphp/ast/DiagnosticsPIL.h"

using namespace polar;
using namespace lowering;

//===----------------------------------------------------------------------===//
// PILGenFunction Class implementation
//===----------------------------------------------------------------------===//

PILGenFunction::PILGenFunction(PILGenModule &SGM, PILFunction &F,
                               DeclContext *DC)
   : SGM(SGM), F(F), silConv(SGM.M), FunctionDC(DC),
     StartOfPostmatter(F.end()), B(*this), OpenedArchetypesTracker(&F),
     CurrentPILLoc(F.getLocation()), Cleanups(*this),
     StatsTracer(SGM.M.getAstContext().Stats, "PILGen-function", &F) {
   assert(DC && "creating SGF without a DeclContext?");
   B.setInsertionPoint(createBasicBlock());
   B.setCurrentDebugScope(F.getDebugScope());
   B.setOpenedArchetypesTracker(&OpenedArchetypesTracker);
}

/// PILGenFunction destructor - called after the entire function's AST has been
/// visited.  This handles "falling off the end of the function" logic.
PILGenFunction::~PILGenFunction() {
   // If the end of the function isn't terminated, we screwed up somewhere.
   assert(!B.hasValidInsertionPoint() &&
          "PILGenFunction did not terminate function?!");

   // If we didn't clean up the rethrow destination, we screwed up somewhere.
   assert(!ThrowDest.isValid() &&
          "PILGenFunction did not emit throw destination");
}

//===----------------------------------------------------------------------===//
// Function emission
//===----------------------------------------------------------------------===//

// Get the #function name for a declaration.
DeclName PILGenModule::getMagicFunctionName(DeclContext *dc) {
   // For closures, use the parent name.
   if (auto closure = dyn_cast<AbstractClosureExpr>(dc)) {
      return getMagicFunctionName(closure->getParent());
   }
   if (auto absFunc = dyn_cast<AbstractFunctionDecl>(dc)) {
      // If this is an accessor, use the name of the storage.
      if (auto accessor = dyn_cast<AccessorDecl>(absFunc))
         return accessor->getStorage()->getFullName();
      if (auto func = dyn_cast<FuncDecl>(absFunc)) {
         // If this is a defer body, use the parent name.
         if (func->isDeferBody()) {
            return getMagicFunctionName(func->getParent());
         }
      }

      return absFunc->getFullName();
   }
   if (auto init = dyn_cast<Initializer>(dc)) {
      return getMagicFunctionName(init->getParent());
   }
   if (auto nominal = dyn_cast<NominalTypeDecl>(dc)) {
      return nominal->getName();
   }
   if (auto tl = dyn_cast<TopLevelCodeDecl>(dc)) {
      return tl->getModuleContext()->getName();
   }
   if (auto fu = dyn_cast<FileUnit>(dc)) {
      return fu->getParentModule()->getName();
   }
   if (auto m = dyn_cast<ModuleDecl>(dc)) {
      return m->getName();
   }
   if (auto e = dyn_cast<ExtensionDecl>(dc)) {
      assert(e->getExtendedNominal() && "extension for nonnominal");
      return e->getExtendedNominal()->getName();
   }
   if (auto EED = dyn_cast<EnumElementDecl>(dc)) {
      return EED->getFullName();
   }
   if (auto SD = dyn_cast<SubscriptDecl>(dc)) {
      return SD->getFullName();
   }
   llvm_unreachable("unexpected #function context");
}

DeclName PILGenModule::getMagicFunctionName(PILDeclRef ref) {
   switch (ref.kind) {
      case PILDeclRef::Kind::Func:
         if (auto closure = ref.getAbstractClosureExpr())
            return getMagicFunctionName(closure);
         return getMagicFunctionName(cast<FuncDecl>(ref.getDecl()));
      case PILDeclRef::Kind::Initializer:
      case PILDeclRef::Kind::Allocator:
         return getMagicFunctionName(cast<ConstructorDecl>(ref.getDecl()));
      case PILDeclRef::Kind::Deallocator:
      case PILDeclRef::Kind::Destroyer:
         return getMagicFunctionName(cast<DestructorDecl>(ref.getDecl()));
      case PILDeclRef::Kind::GlobalAccessor:
         return getMagicFunctionName(cast<VarDecl>(ref.getDecl())->getDeclContext());
      case PILDeclRef::Kind::DefaultArgGenerator:
         return getMagicFunctionName(cast<DeclContext>(ref.getDecl()));
      case PILDeclRef::Kind::StoredPropertyInitializer:
      case PILDeclRef::Kind::PropertyWrapperBackingInitializer:
         return getMagicFunctionName(cast<VarDecl>(ref.getDecl())->getDeclContext());
      case PILDeclRef::Kind::IVarInitializer:
         return getMagicFunctionName(cast<ClassDecl>(ref.getDecl()));
      case PILDeclRef::Kind::IVarDestroyer:
         return getMagicFunctionName(cast<ClassDecl>(ref.getDecl()));
      case PILDeclRef::Kind::EnumElement:
         return getMagicFunctionName(cast<EnumElementDecl>(ref.getDecl())
                                        ->getDeclContext());
   }

   llvm_unreachable("Unhandled PILDeclRefKind in switch.");
}

std::tuple<ManagedValue, PILType>
PILGenFunction::emitSiblingMethodRef(PILLocation loc,
                                     PILValue selfValue,
                                     PILDeclRef methodConstant,
                                     SubstitutionMap subMap) {
   PILValue methodValue;

   // If the method is dynamic, access it through runtime-hookable virtual
   // dispatch (viz. objc_msgSend for now).
   // @todo
//   if (methodConstant.hasDecl()
//       && methodConstant.getDecl()->isObjCDynamic()) {
//      methodValue =
//         emitDynamicMethodRef(
//            loc, methodConstant,
//            SGM.Types.getConstantInfo(getTypeExpansionContext(), methodConstant)
//               .PILFnType)
//            .getValue();
//   } else {
   methodValue = emitGlobalFunctionRef(loc, methodConstant);
//   }

   PILType methodTy = methodValue->getType();

   // Specialize the generic method.
   methodTy =
      methodTy.substGenericArgs(SGM.M, subMap, getTypeExpansionContext());

   return std::make_tuple(ManagedValue::forUnmanaged(methodValue),
                          methodTy);
}

void PILGenFunction::emitCaptures(PILLocation loc,
                                  PILDeclRef closure,
                                  CaptureEmission purpose,
                                  SmallVectorImpl<ManagedValue> &capturedArgs) {
   auto captureInfo = SGM.Types.getLoweredLocalCaptures(closure);
   // For boxed captures, we need to mark the contained variables as having
   // escaped for DI diagnostics.
   SmallVector<PILValue, 2> escapesToMark;

   // Partial applications take ownership of the context parameters, so we'll
   // need to pass ownership rather than merely guaranteeing parameters.
   bool canGuarantee;
   switch (purpose) {
      case CaptureEmission::PartialApplication:
         canGuarantee = false;
         break;
      case CaptureEmission::ImmediateApplication:
         canGuarantee = true;
         break;
   }

   auto expansion = getTypeExpansionContext();

   for (auto capture : captureInfo.getCaptures()) {
      if (capture.isDynamicSelfMetadata()) {
         // The parameter type is the static Self type, but the value we
         // want to pass is the dynamic Self type, so upcast it.
         auto dynamicSelfMetatype = MetatypeType::get(
            captureInfo.getDynamicSelfType());
         PILType dynamicPILType = getLoweredType(dynamicSelfMetatype);

         PILValue value = B.createMetatype(loc, dynamicPILType);
         capturedArgs.push_back(ManagedValue::forUnmanaged(value));
         continue;
      }

      if (capture.isOpaqueValue()) {
         OpaqueValueExpr *opaqueValue = capture.getOpaqueValue();
         capturedArgs.push_back(
            emitRValueAsSingleValue(opaqueValue).ensurePlusOne(*this, loc));
         continue;
      }

      auto *vd = cast<VarDecl>(capture.getDecl());
      auto type = FunctionDC->mapTypeIntoContext(
         vd->getInterfaceType());
      auto valueType = FunctionDC->mapTypeIntoContext(
         vd->getValueInterfaceType());

      //
      // If we haven't emitted the captured value yet, we're forming a closure
      // to a local function before all of its captures have been emitted. Eg,
      //
      // func f() { g() } // transitive capture of 'x'
      // f() // closure formed here
      // var x = 123 // 'x' defined here
      // func g() { print(x) } // 'x' captured here
      //
      auto found = VarLocs.find(vd);
      if (found == VarLocs.end()) {
         auto &Diags = getAstContext().Diags;

         SourceLoc loc;
         bool isDeferBody;
         if (closure.kind == PILDeclRef::Kind::DefaultArgGenerator) {
            auto *param = getParameterAt(closure.getDecl(),
                                         closure.defaultArgIndex);
            loc = param->getLoc();
            isDeferBody = false;
         } else {
            auto f = *closure.getAnyFunctionRef();
            loc = f.getLoc();
            isDeferBody = f.isDeferBody();
         }

         Diags.diagnose(loc,
                        isDeferBody
                        ? diag::capture_before_declaration_defer
                        : diag::capture_before_declaration,
                        vd->getBaseName().getIdentifier());
         Diags.diagnose(vd->getLoc(), diag::captured_value_declared_here);
         Diags.diagnose(capture.getLoc(), diag::value_captured_here);

         // Emit an 'undef' of the correct type.
         switch (SGM.Types.getDeclCaptureKind(capture, expansion)) {
            case CaptureKind::Constant:
               capturedArgs.push_back(emitUndef(getLoweredType(type)));
               break;
            case CaptureKind::StorageAddress:
               capturedArgs.push_back(emitUndef(getLoweredType(type).getAddressType()));
               break;
            case CaptureKind::Box: {
               auto boxTy = SGM.Types.getContextBoxTypeForCapture(
                  vd,
                  SGM.Types.getLoweredRValueType(TypeExpansionContext::minimal(),
                                                 type),
                  FunctionDC->getGenericEnvironmentOfContext(),
                  /*mutable*/ true);
               capturedArgs.push_back(emitUndef(boxTy));
               break;
            }
         }
         continue;
      }

      // Get an address value for a PILValue if it is address only in an type
      // expansion context without opaque archetype substitution.
      auto getAddressValue = [&](PILValue entryValue) -> PILValue {
         if (SGM.Types
                .getTypeLowering(
                   valueType,
                   TypeExpansionContext::noOpaqueTypeArchetypesSubstitution(
                      expansion.getResilienceExpansion()))
                .isAddressOnly() &&
             !entryValue->getType().isAddress()) {

            auto addr = emitTemporaryAllocation(vd, entryValue->getType());
            auto val = B.emitCopyValueOperation(vd, entryValue);
            auto &lowering = getTypeLowering(entryValue->getType());
            lowering.emitStore(B, vd, val, addr, StoreOwnershipQualifier::Init);
            entryValue = addr;
            enterDestroyCleanup(addr);
         }
         return entryValue;
      };

      auto Entry = found->second;
      switch (SGM.Types.getDeclCaptureKind(capture, expansion)) {
         case CaptureKind::Constant: {
            // let declarations.
            auto &tl = getTypeLowering(valueType);
            PILValue Val = Entry.value;

            if (!Val->getType().isAddress()) {
               // Our 'let' binding can guarantee the lifetime for the callee,
               // if we don't need to do anything more to it.
               if (canGuarantee && !vd->getInterfaceType()->is<ReferenceStorageType>()) {
                  auto guaranteed = ManagedValue::forUnmanaged(Val).borrow(*this, loc);
                  capturedArgs.push_back(guaranteed);
                  break;
               }

               // Just retain a by-val let.
               Val = B.emitCopyValueOperation(loc, Val);
            } else {
               // If we have a mutable binding for a 'let', such as 'self' in an
               // 'init' method, load it.
               Val = emitLoad(loc, Val, tl, SGFContext(), IsNotTake).forward(*this);
            }

            // If we're capturing an unowned pointer by value, we will have just
            // loaded it into a normal retained class pointer, but we capture it as
            // an unowned pointer.  Convert back now.
            if (vd->getInterfaceType()->is<ReferenceStorageType>())
               Val = emitConversionFromSemanticValue(loc, Val, getLoweredType(type));

            capturedArgs.push_back(emitManagedRValueWithCleanup(Val));
            break;
         }

         case CaptureKind::StorageAddress: {
            auto entryValue = getAddressValue(Entry.value);
            // No-escaping stored declarations are captured as the
            // address of the value.
            assert(entryValue->getType().isAddress() && "no address for captured var!");
            capturedArgs.push_back(ManagedValue::forLValue(entryValue));
            break;
         }

         case CaptureKind::Box: {
            auto entryValue = getAddressValue(Entry.value);
            // LValues are captured as both the box owning the value and the
            // address of the value.
            assert(entryValue->getType().isAddress() && "no address for captured var!");
            // Boxes of opaque return values stay opaque.
            auto minimalLoweredType = SGM.Types.getLoweredRValueType(
               TypeExpansionContext::minimal(), type->getCanonicalType());
            // If this is a boxed variable, we can use it directly.
            if (Entry.box &&
                entryValue->getType().getAstType() == minimalLoweredType) {
               // We can guarantee our own box to the callee.
               if (canGuarantee) {
                  capturedArgs.push_back(
                     ManagedValue::forUnmanaged(Entry.box).borrow(*this, loc));
               } else {
                  capturedArgs.push_back(emitManagedRetain(loc, Entry.box));
               }
               escapesToMark.push_back(entryValue);
            } else {
               // Address only 'let' values are passed by box.  This isn't great, in
               // that a variable captured by multiple closures will be boxed for each
               // one.  This could be improved by doing an "isCaptured" analysis when
               // emitting address-only let constants, and emit them into an alloc_box
               // like a variable instead of into an alloc_stack.
               //
               // TODO: This might not be profitable anymore with guaranteed captures,
               // since we could conceivably forward the copied value into the
               // closure context and pass it down to the partially applied function
               // in-place.
               // TODO: Use immutable box for immutable captures.
               auto boxTy = SGM.Types.getContextBoxTypeForCapture(
                  vd, minimalLoweredType, FunctionDC->getGenericEnvironmentOfContext(),
                  /*mutable*/ true);

               AllocBoxInst *allocBox = B.createAllocBox(loc, boxTy);
               ProjectBoxInst *boxAddress = B.createProjectBox(loc, allocBox, 0);
               B.createCopyAddr(loc, entryValue, boxAddress, IsNotTake,
                                IsInitialization);
               if (canGuarantee)
                  capturedArgs.push_back(
                     emitManagedRValueWithCleanup(allocBox).borrow(*this, loc));
               else
                  capturedArgs.push_back(emitManagedRValueWithCleanup(allocBox));
            }

            break;
         }
      }
   }

   // Mark box addresses as captured for DI purposes. The values must have
   // been fully initialized before we close over them.
   if (!escapesToMark.empty()) {
      B.createMarkFunctionEscape(loc, escapesToMark);
   }
}

ManagedValue
PILGenFunction::emitClosureValue(PILLocation loc, PILDeclRef constant,
                                 CanType expectedType,
                                 SubstitutionMap subs) {
   auto loweredCaptureInfo = SGM.Types.getLoweredLocalCaptures(constant);

   auto constantInfo = getConstantInfo(getTypeExpansionContext(), constant);
   PILValue functionRef = emitGlobalFunctionRef(loc, constant, constantInfo);
   PILType functionTy = functionRef->getType();

   // Apply substitutions.
   auto pft = constantInfo.PILFnType;

   auto closure = *constant.getAnyFunctionRef();
   auto *dc = closure.getAsDeclContext()->getParent();
   if (dc->isLocalContext() && !loweredCaptureInfo.hasGenericParamCaptures()) {
      // If the lowered function type is not polymorphic but we were given
      // substitutions, we have a closure in a generic context which does not
      // capture generic parameters. Just drop the substitutions.
      subs = { };
   } else if (closure.getAbstractClosureExpr()) {
      // If we have a closure expression in generic context, Sema won't give
      // us substitutions, so we just use the forwarding substitutions from
      // context.
      subs = getForwardingSubstitutionMap();
   }

   bool wasSpecialized = false;
   if (!subs.empty()) {
      auto specialized =
         pft->substGenericArgs(F.getModule(), subs, getTypeExpansionContext());
      functionTy = PILType::getPrimitiveObjectType(specialized);
      wasSpecialized = true;
   }

   // If we're in top-level code, we don't need to physically capture script
   // globals, but we still need to mark them as escaping so that DI can flag
   // uninitialized uses.
   if (this == SGM.TopLevelSGF) {
      auto captureInfo = closure.getCaptureInfo();
      SGM.emitMarkFunctionEscapeForTopLevelCodeGlobals(
         loc, captureInfo);
   }

   if (loweredCaptureInfo.getCaptures().empty() && !wasSpecialized) {
      auto result = ManagedValue::forUnmanaged(functionRef);
      return emitOrigToSubstValue(loc, result,
                                  AbstractionPattern(expectedType),
                                  expectedType);
   }

   SmallVector<ManagedValue, 4> capturedArgs;
   emitCaptures(loc, constant, CaptureEmission::PartialApplication,
                capturedArgs);

   // The partial application takes ownership of the context parameters.
   SmallVector<PILValue, 4> forwardedArgs;
   for (auto capture : capturedArgs)
      forwardedArgs.push_back(capture.forward(*this));

   auto calleeConvention = ParameterConvention::Direct_Guaranteed;

   auto toClosure =
      B.createPartialApply(loc, functionRef, subs, forwardedArgs,
                           calleeConvention);
   auto result = emitManagedRValueWithCleanup(toClosure);

   // Get the lowered AST types:
   //  - the original type
   auto origFormalType = AbstractionPattern(constantInfo.LoweredType);

   // - the substituted type
   auto substFormalType = expectedType;

   // Generalize if necessary.
   result = emitOrigToSubstValue(loc, result, origFormalType,
                                 substFormalType);

   return result;
}

void PILGenFunction::emitFunction(FuncDecl *fd) {
   MagicFunctionName = PILGenModule::getMagicFunctionName(fd);

   auto captureInfo = SGM.M.Types.getLoweredLocalCaptures(PILDeclRef(fd));
   emitProlog(captureInfo, fd->getParameters(), fd->getImplicitSelfDecl(), fd,
              fd->getResultInterfaceType(), fd->hasThrows(), fd->getThrowsLoc());
   Type resultTy = fd->mapTypeIntoContext(fd->getResultInterfaceType());
   prepareEpilog(resultTy, fd->hasThrows(), CleanupLocation(fd));

   emitProfilerIncrement(fd->getBody());
   emitStmt(fd->getBody());

   emitEpilog(fd);

   mergeCleanupBlocks();
}

void PILGenFunction::emitClosure(AbstractClosureExpr *ace) {
   MagicFunctionName = PILGenModule::getMagicFunctionName(ace);

   auto resultIfaceTy = ace->getResultType()->mapTypeOutOfContext();
   auto captureInfo = SGM.M.Types.getLoweredLocalCaptures(
      PILDeclRef(ace));
   emitProlog(captureInfo, ace->getParameters(), /*selfParam=*/nullptr,
              ace, resultIfaceTy, ace->isBodyThrowing(), ace->getLoc());
   prepareEpilog(ace->getResultType(), ace->isBodyThrowing(),
                 CleanupLocation(ace));
   emitProfilerIncrement(ace);
   if (auto *ce = dyn_cast<ClosureExpr>(ace)) {
      emitStmt(ce->getBody());
   } else {
      auto *autoclosure = cast<AutoClosureExpr>(ace);
      // Closure expressions implicitly return the result of their body
      // expression.
      emitReturnExpr(ImplicitReturnLocation(ace),
                     autoclosure->getSingleExpressionBody());
   }
   emitEpilog(ace);
}

void PILGenFunction::emitArtificialTopLevel(ClassDecl *mainClass) {
   // Load argc and argv from the entry point arguments.
   PILValue argc = F.begin()->getArgument(0);
   PILValue argv = F.begin()->getArgument(1);

   switch (mainClass->getArtificialMainKind()) {
      case ArtificialMainKind::UIApplicationMain: {
         // Emit a UIKit main.
         // return UIApplicationMain(C_ARGC, C_ARGV, nil, ClassName);

         CanType NSStringTy = SGM.Types.getNSStringType();
         CanType OptNSStringTy
            = OptionalType::get(NSStringTy)->getCanonicalType();

         // Look up UIApplicationMain.
         // FIXME: Doing an AST lookup here is gross and not entirely sound;
         // we're getting away with it because the types are guaranteed to already
         // be imported.
         AstContext &ctx = getAstContext();

         std::pair<Identifier, SourceLoc> UIKitName =
            {ctx.getIdentifier("UIKit"), SourceLoc()};

         ModuleDecl *UIKit = ctx
            .getClangModuleLoader()
            ->loadModule(SourceLoc(), UIKitName);
         assert(UIKit && "couldn't find UIKit objc module?!");
         SmallVector<ValueDecl *, 1> results;
         UIKit->lookupQualified(UIKit,
                                ctx.getIdentifier("UIApplicationMain"),
                                NL_QualifiedDefault,
                                results);
         assert(results.size() == 1
                && "couldn't find a unique UIApplicationMain in the UIKit ObjC "
                   "module?!");

         ValueDecl *UIApplicationMainDecl = results.front();

         auto mainRef = PILDeclRef(UIApplicationMainDecl).asForeign();
         PILGenFunctionBuilder builder(SGM);
         auto UIApplicationMainFn =
            builder.getOrCreateFunction(mainClass, mainRef, NotForDefinition);
         auto fnTy = UIApplicationMainFn->getLoweredFunctionType();
         PILFunctionConventions fnConv(fnTy, SGM.M);

         // Get the class name as a string using NSStringFromClass.
         CanType mainClassTy = mainClass->getDeclaredInterfaceType()
            ->getCanonicalType();
         CanType mainClassMetaty = CanMetatypeType::get(mainClassTy,
                                                        MetatypeRepresentation::ObjC);
         CanType anyObjectTy = ctx.getAnyObjectType();
         CanType anyObjectMetaTy = CanExistentialMetatypeType::get(anyObjectTy,
                                                                   MetatypeRepresentation::ObjC);

         auto NSStringFromClassType = PILFunctionType::get(nullptr,
                                                           PILFunctionType::ExtInfo()
                                                              .withRepresentation(PILFunctionType::Representation::
                                                                                  CFunctionPointer),
                                                           PILCoroutineKind::None,
                                                           ParameterConvention::Direct_Unowned,
                                                           PILParameterInfo(anyObjectMetaTy,
                                                                            ParameterConvention::Direct_Unowned),
            /*yields*/ {},
                                                           PILResultInfo(OptNSStringTy,
                                                                         ResultConvention::Autoreleased),
            /*error result*/ None,
                                                           SubstitutionMap(), false,
                                                           ctx);
         auto NSStringFromClassFn = builder.getOrCreateFunction(
            mainClass, "NSStringFromClass", PILLinkage::PublicExternal,
            NSStringFromClassType, IsBare, IsTransparent, IsNotSerialized,
            IsNotDynamic);
         auto NSStringFromClass = B.createFunctionRef(mainClass, NSStringFromClassFn);
         PILValue metaTy = B.createMetatype(mainClass,
                                            PILType::getPrimitiveObjectType(mainClassMetaty));
         metaTy = B.createInitExistentialMetatype(mainClass, metaTy,
                                                  PILType::getPrimitiveObjectType(anyObjectMetaTy),
                                                  {});
         PILValue optNameValue = B.createApply(
            mainClass, NSStringFromClass, {}, metaTy);
         ManagedValue optName = emitManagedRValueWithCleanup(optNameValue);

         // Fix up the string parameters to have the right type.
         PILType nameArgTy = fnConv.getPILArgumentType(3);
         assert(nameArgTy == fnConv.getPILArgumentType(2));
         (void)nameArgTy;
         assert(optName.getType() == nameArgTy);
         PILValue nilValue =
            getOptionalNoneValue(mainClass, getTypeLowering(OptNSStringTy));

         // Fix up argv to have the right type.
         auto argvTy = fnConv.getPILArgumentType(1);

         PILType unwrappedTy = argvTy;
         if (Type innerTy = argvTy.getAstType()->getOptionalObjectType()) {
            auto canInnerTy = innerTy->getCanonicalType();
            unwrappedTy = PILType::getPrimitiveObjectType(canInnerTy);
         }

         auto managedArgv = ManagedValue::forUnmanaged(argv);

         if (unwrappedTy != argv->getType()) {
            auto converted =
               emitPointerToPointer(mainClass, managedArgv,
                                    argv->getType().getAstType(),
                                    unwrappedTy.getAstType());
            managedArgv = std::move(converted).getAsSingleValue(*this, mainClass);
         }

         if (unwrappedTy != argvTy) {
            managedArgv = getOptionalSomeValue(mainClass, managedArgv,
                                               getTypeLowering(argvTy));
         }

         auto UIApplicationMain = B.createFunctionRef(mainClass, UIApplicationMainFn);

         PILValue args[] = {argc, managedArgv.getValue(), nilValue,
                            optName.getValue()};

         B.createApply(mainClass, UIApplicationMain, SubstitutionMap(), args);
         PILValue r = B.createIntegerLiteral(mainClass,
                                             PILType::getBuiltinIntegerType(32, ctx), 0);
         auto rType = F.getConventions().getSinglePILResultType();
         if (r->getType() != rType)
            r = B.createStruct(mainClass, rType, r);

         Cleanups.emitCleanupsForReturn(mainClass, NotForUnwind);
         B.createReturn(mainClass, r);
         return;
      }

      case ArtificialMainKind::NSApplicationMain: {
         // Emit an AppKit main.
         // return NSApplicationMain(C_ARGC, C_ARGV);

         PILParameterInfo argTypes[] = {
            PILParameterInfo(argc->getType().getAstType(),
                             ParameterConvention::Direct_Unowned),
            PILParameterInfo(argv->getType().getAstType(),
                             ParameterConvention::Direct_Unowned),
         };
         auto NSApplicationMainType = PILFunctionType::get(nullptr,
                                                           PILFunctionType::ExtInfo()
                                                              // Should be C calling convention, but NSApplicationMain
                                                              // has an overlay to fix the type of argv.
                                                              .withRepresentation(PILFunctionType::Representation::Thin),
                                                           PILCoroutineKind::None,
                                                           ParameterConvention::Direct_Unowned,
                                                           argTypes,
            /*yields*/ {},
                                                           PILResultInfo(argc->getType().getAstType(),
                                                                         ResultConvention::Unowned),
            /*error result*/ None,
                                                           SubstitutionMap(), false,
                                                           getAstContext());

         PILGenFunctionBuilder builder(SGM);
         auto NSApplicationMainFn = builder.getOrCreateFunction(
            mainClass, "NSApplicationMain", PILLinkage::PublicExternal,
            NSApplicationMainType, IsBare, IsTransparent, IsNotSerialized,
            IsNotDynamic);

         auto NSApplicationMain = B.createFunctionRef(mainClass, NSApplicationMainFn);
         PILValue args[] = { argc, argv };

         B.createApply(mainClass, NSApplicationMain, SubstitutionMap(), args);
         PILValue r = B.createIntegerLiteral(mainClass,
                                             PILType::getBuiltinIntegerType(32, getAstContext()), 0);
         auto rType = F.getConventions().getSinglePILResultType();
         if (r->getType() != rType)
            r = B.createStruct(mainClass, rType, r);
         B.createReturn(mainClass, r);
         return;
      }
   }
}

#ifndef NDEBUG
/// If \c false, \c function is either a declaration that inherently cannot
/// capture variables, or it is in a context it cannot capture variables from.
/// In either case, it is expected that Sema may not have computed its
/// \c CaptureInfo.
///
/// This call exists for use in assertions; do not use it to skip capture
/// processing.
static bool canCaptureFromParent(PILDeclRef function) {
   switch (function.kind) {
      case PILDeclRef::Kind::StoredPropertyInitializer:
      case PILDeclRef::Kind::PropertyWrapperBackingInitializer:
         return false;

      default:
         if (function.hasDecl()) {
            if (auto dc = dyn_cast<DeclContext>(function.getDecl())) {
               return TypeConverter::canCaptureFromParent(dc);
            }
         }
         return false;
   }
}
#endif

void PILGenFunction::emitGeneratorFunction(PILDeclRef function, Expr *value,
                                           bool EmitProfilerIncrement) {
   auto *dc = function.getDecl()->getInnermostDeclContext();
   MagicFunctionName = PILGenModule::getMagicFunctionName(function);

   RegularLocation Loc(value);
   Loc.markAutoGenerated();

   // Default argument generators of function typed values return noescape
   // functions. Strip the escape to noescape function conversion.
   if (function.kind == PILDeclRef::Kind::DefaultArgGenerator) {
      if (auto funType = value->getType()->getAs<AnyFunctionType>()) {
         if (funType->getExtInfo().isNoEscape()) {
            auto conv = cast<FunctionConversionExpr>(value);
            value = conv->getSubExpr();
            assert(funType->withExtInfo(funType->getExtInfo().withNoEscape(false))
                      ->isEqual(value->getType()));
         }
      }
   }

   // For a property wrapper backing initializer, form a parameter list
   // containing the wrapped value.
   ParameterList *params = nullptr;
   if (function.kind == PILDeclRef::Kind::PropertyWrapperBackingInitializer) {
      auto &ctx = getAstContext();
      auto param = new (ctx) ParamDecl(SourceLoc(), SourceLoc(),
                                       ctx.getIdentifier("$input_value"),
                                       SourceLoc(),
                                       ctx.getIdentifier("$input_value"),
                                       dc);
      param->setSpecifier(ParamSpecifier::Owned);
      param->setInterfaceType(function.getDecl()->getInterfaceType());

      params = ParameterList::create(ctx, SourceLoc(), {param}, SourceLoc());
   }

   CaptureInfo captureInfo;
   if (function.getAnyFunctionRef())
      captureInfo = SGM.M.Types.getLoweredLocalCaptures(function);
   else {
      assert(!canCaptureFromParent(function));
      captureInfo = CaptureInfo::empty();
   }

   auto interfaceType = value->getType()->mapTypeOutOfContext();
   emitProlog(captureInfo, params, /*selfParam=*/nullptr,
              dc, interfaceType, /*throws=*/false, SourceLoc());
   if (EmitProfilerIncrement)
      emitProfilerIncrement(value);
   prepareEpilog(value->getType(), false, CleanupLocation::get(Loc));

   {
      llvm::Optional<PILGenFunction::OpaqueValueRAII> opaqueValue;

      // For a property wrapper backing initializer, bind the opaque value used
      // in the initializer expression to the given parameter.
      if (function.kind == PILDeclRef::Kind::PropertyWrapperBackingInitializer) {
         auto var = cast<VarDecl>(function.getDecl());
         auto wrappedInfo = var->getPropertyWrapperBackingPropertyInfo();
         auto param = params->get(0);
         opaqueValue.emplace(*this, wrappedInfo.underlyingValue,
                             maybeEmitValueOfLocalVarDecl(param));

         assert(value == wrappedInfo.initializeFromOriginal);
      }

      emitReturnExpr(Loc, value);
   }

   emitEpilog(Loc);
   mergeCleanupBlocks();
}

void PILGenFunction::emitGeneratorFunction(PILDeclRef function, VarDecl *var) {
   MagicFunctionName = PILGenModule::getMagicFunctionName(function);

   RegularLocation loc(var);
   loc.markAutoGenerated();

   auto decl = function.getAbstractFunctionDecl();
   auto *dc = decl->getInnermostDeclContext();
   auto interfaceType = var->getValueInterfaceType();
   auto varType = var->getType();

   // If this is the backing storage for a property with an attached
   // wrapper that was initialized with '=', the stored property initializer
   // will be in terms of the original property's type.
   if (auto originalProperty = var->getOriginalWrappedProperty()) {
      if (originalProperty->isPropertyWrapperInitializedWithInitialValue()) {
         interfaceType = originalProperty->getValueInterfaceType();
         varType = originalProperty->getType();
      }
   }

   emitProlog(/*paramList*/ nullptr, /*selfParam*/ nullptr, interfaceType, dc,
      /*throws=*/false, SourceLoc());
   prepareEpilog(varType, false, CleanupLocation::get(loc));

   auto pbd = var->getParentPatternBinding();
   const auto i = pbd->getPatternEntryIndexForVarDecl(var);
   auto *anchorVar = pbd->getAnchoringVarDecl(i);
   auto subs = getForwardingSubstitutionMap();
   auto contextualType = dc->mapTypeIntoContext(interfaceType);
   auto resultType = contextualType->getCanonicalType();
   auto origResultType = AbstractionPattern(resultType);

   SmallVector<PILValue, 4> directResults;

   if (F.getConventions().hasIndirectPILResults()) {
      Scope scope(Cleanups, CleanupLocation(var));

      SmallVector<CleanupHandle, 4> cleanups;
      auto init = prepareIndirectResultInit(resultType, directResults, cleanups);

      emitApplyOfStoredPropertyInitializer(loc, anchorVar, subs, resultType,
                                           origResultType,
                                           SGFContext(init.get()));

      for (auto cleanup : cleanups) {
         Cleanups.forwardCleanup(cleanup);
      }
   } else {
      Scope scope(Cleanups, CleanupLocation(var));

      // If we have no indirect results, just return the result.
      auto result = emitApplyOfStoredPropertyInitializer(loc, anchorVar, subs,
                                                         resultType,
                                                         origResultType,
                                                         SGFContext())
         .ensurePlusOne(*this, loc);
      std::move(result).forwardAll(*this, directResults);
   }

   Cleanups.emitBranchAndCleanups(ReturnDest, loc, directResults);
   emitEpilog(loc);
}

static PILLocation getLocation(AstNode Node) {
   if (auto *E = Node.dyn_cast<Expr *>())
      return E;
   else if (auto *S = Node.dyn_cast<Stmt *>())
      return S;
   else if (auto *D = Node.dyn_cast<Decl *>())
      return D;
   else
      llvm_unreachable("unsupported AstNode");
}

void PILGenFunction::emitProfilerIncrement(AstNode N) {
   // Ignore functions which aren't set up for instrumentation.
   PILProfiler *SP = F.getProfiler();
   if (!SP)
      return;
   if (!SP->hasRegionCounters() || !getModule().getOptions().UseProfile.empty())
      return;

   auto &C = B.getAstContext();
   const auto &RegionCounterMap = SP->getRegionCounterMap();
   auto CounterIt = RegionCounterMap.find(N);

   // TODO: Assert that this cannot happen (rdar://42792053).
   if (CounterIt == RegionCounterMap.end())
      return;

   auto Int32Ty = getLoweredType(BuiltinIntegerType::get(32, C));
   auto Int64Ty = getLoweredType(BuiltinIntegerType::get(64, C));

   PILLocation Loc = getLocation(N);
   PILValue Args[] = {
      // The intrinsic must refer to the function profiling name var, which is
      // inaccessible during PILGen. Rely on irgen to rewrite the function name.
      B.createStringLiteral(Loc, SP->getPGOFuncName(),
                            StringLiteralInst::Encoding::UTF8),
      B.createIntegerLiteral(Loc, Int64Ty, SP->getPGOFuncHash()),
      B.createIntegerLiteral(Loc, Int32Ty, SP->getNumRegionCounters()),
      B.createIntegerLiteral(Loc, Int32Ty, CounterIt->second)};
   B.createBuiltin(Loc, C.getIdentifier("int_instrprof_increment"),
                   SGM.Types.getEmptyTupleType(), {}, Args);
}

ProfileCounter PILGenFunction::loadProfilerCount(AstNode Node) const {
   if (PILProfiler *SP = F.getProfiler())
      return SP->getExecutionCount(Node);
   return ProfileCounter();
}

Optional<AstNode> PILGenFunction::getPGOParent(AstNode Node) const {
   if (PILProfiler *SP = F.getProfiler())
      return SP->getPGOParent(Node);
   return None;
}

PILValue PILGenFunction::emitUnwrapIntegerResult(PILLocation loc,
                                                 PILValue value) {
   // This is a loop because we want to handle types that wrap integer types,
   // like ObjCBool (which may be Bool or Int8).
   while (!value->getType().is<BuiltinIntegerType>()) {
      auto structDecl = value->getType().getStructOrBoundGenericStruct();
      assert(structDecl && "value for error result wasn't of struct type!");
      assert(structDecl->getStoredProperties().size() == 1);
      auto property = structDecl->getStoredProperties()[0];
      value = B.createStructExtract(loc, value, property);
   }

   return value;
}
