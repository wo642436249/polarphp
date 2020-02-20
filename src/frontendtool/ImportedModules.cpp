//===--- ImportedModules.cpp -- generates the list of imported modules ----===//
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

#include "polarphp/ast/AstContext.h"
#include "polarphp/ast/Decl.h"
#include "polarphp/ast/DiagnosticEngine.h"
#include "polarphp/ast/DiagnosticsFrontend.h"
#include "polarphp/ast/Module.h"
#include "polarphp/basic/LLVM.h"
#include "polarphp/clangimporter/ClangImporter.h"
#include "polarphp/frontend/FrontendOptions.h"
#include "polarphp/frontendtool/internal/ImportedModules.h"
#include "clang/Basic/Module.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FileSystem.h"

using namespace polar;

static StringRef getTopLevelName(const clang::Module *module) {
   return module->getTopLevelModule()->Name;
}

static void findAllClangImports(const clang::Module *module,
                                llvm::SetVector<StringRef> &modules) {
   for (auto imported : module->Imports) {
      modules.insert(getTopLevelName(imported));
   }

   for (auto sub : module->submodules()) {
      findAllClangImports(sub, modules);
   }
}

bool polar::emitImportedModules(AstContext &Context, ModuleDecl *mainModule,
                                const FrontendOptions &opts) {

   std::string path = opts.InputsAndOutputs.getSingleOutputFilename();
   std::error_code EC;
   llvm::raw_fd_ostream out(path, EC, llvm::sys::fs::F_None);

   if (out.has_error() || EC) {
      Context.Diags.diagnose(SourceLoc(), diag::error_opening_output, path,
                             EC.message());
      out.clear_error();
      return true;
   }

   llvm::SetVector<StringRef> Modules;

   // Find the imports in the main Swift code.
   llvm::SmallVector<Decl *, 32> Decls;
   mainModule->getDisplayDecls(Decls);
   for (auto D : Decls) {
      auto ID = dyn_cast<ImportDecl>(D);
      if (!ID)
         continue;

      auto accessPath = ID->getModulePath();
      // only the top-level name is needed (i.e. A in A.B.C)
      Modules.insert(accessPath[0].first.str());
   }

   // And now look in the C code we're possibly using.
   /// TODO
//   auto clangImporter =
//      static_cast<ClangImporter *>(Context.getClangModuleLoader());

//   StringRef implicitHeaderPath = opts.ImplicitObjCHeaderPath;
//   if (!implicitHeaderPath.empty()) {
//      if (!clangImporter->importBridgingHeader(implicitHeaderPath, mainModule)) {
//         ModuleDecl::ImportFilter importFilter;
//         importFilter |= ModuleDecl::ImportFilterKind::Public;
//         importFilter |= ModuleDecl::ImportFilterKind::Private;
//         importFilter |= ModuleDecl::ImportFilterKind::ImplementationOnly;
//
//         SmallVector<ModuleDecl::ImportedModule, 16> imported;
//         clangImporter->getImportedHeaderModule()->getImportedModules(
//            imported, importFilter);
//
//         for (auto IM : imported) {
//            if (auto clangModule = IM.second->findUnderlyingClangModule())
//               Modules.insert(getTopLevelName(clangModule));
//            else
//               assert(IM.second->isStdlibModule() &&
//                      "unexpected non-stdlib swift module");
//         }
//      }
//   }

//   if (opts.ImportUnderlyingModule) {
//      auto underlyingModule = clangImporter->loadModule(
//         SourceLoc(), std::make_pair(mainModule->getName(), SourceLoc()));
//      if (!underlyingModule) {
//         Context.Diags.diagnose(SourceLoc(),
//                                diag::error_underlying_module_not_found,
//                                mainModule->getName());
//         return true;
//      }
//      auto clangModule = underlyingModule->findUnderlyingClangModule();
//
//      findAllClangImports(clangModule, Modules);
//   }

   for (auto name : Modules) {
      out << name << "\n";
   }

   return false;
}