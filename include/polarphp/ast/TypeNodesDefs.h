//===--- TypeNodes.def - Swift Type AST Metaprogramming ---------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
// This source file is part of the polarphp.org open source project
//
// Copyright (c) 2017 - 2018 polarphp software foundation
// Copyright (c) 2017 - 2018 zzu_softboy <zzu_softboy@163.com>
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://polarphp.org/LICENSE.txt for license information
// See https://polarphp.org/CONTRIBUTORS.txt for the list of polarphp project authors
//
// Created by polarboy on 2019/04/30.
//===----------------------------------------------------------------------===//
//
// This file defines macros used for macro-metaprogramming with types.
//
//===----------------------------------------------------------------------===//

/// TYPE(id, parent)
///   If the type node is not abstract, its enumerator value is
///   TypeKind::id.  The node's class name is 'id##Type', and the name of
///   its base class (in the Type hierarchy) is 'parent' (including
///   the 'Type' suffix).

/// ALWAYS_CANONICAL_TYPE(id, parent)
///   This type is always canonical.  The default behavior is TYPE(id, parent).

/// BUILTIN_TYPE(id, parent)
///   This type is a builtin type. The default behavior is
///   ALWAYS_CANONICAL_TYPE(id, parent).

/// SUGARED_TYPE(id, parent)
///   This type is never canonical.  It provides an efficient accessor,
///   getSinglyDesugaredType(), which removes one level of sugar.  This
///   process does not necessarily converge to the canonical type, however.
///   The default behavior is TYPE(id, parent).
//
// If you add a new sugared type, be sure to test it in PrintAsObjC!

/// UNCHECKED_TYPE(id, parent)
///   This type is not present in valid, type-checked programs.
///   The default behavior is TYPE(id, parent).

/// ARTIFICIAL_TYPE(id, parent)
///   This type is not part of the user-facing type system. It may be
///   used freely in SIL, and it may appear as the type of a
///   declaration in the AST (due to e.g. the application of the
///   [weak] attribute), but it cannot be written directly in normal
///   programs and it cannot be the type of an expression.
///   The default behavior is TYPE(id, parent).

#ifndef ALWAYS_CANONICAL_TYPE
#define ALWAYS_CANONICAL_TYPE(id, parent) TYPE(id, parent)
#endif

#ifndef BUILTIN_TYPE
#define BUILTIN_TYPE(id, parent) ALWAYS_CANONICAL_TYPE(id, parent)
#endif

#ifndef SUGARED_TYPE
#define SUGARED_TYPE(id, parent) TYPE(id, parent)
#endif

#ifndef UNCHECKED_TYPE
#define UNCHECKED_TYPE(id, parent) TYPE(id, parent)
#endif

#ifndef ARTIFICIAL_TYPE
#define ARTIFICIAL_TYPE(id, parent) TYPE(id, parent)
#endif

/// Most metaprograms do not care about abstract types, so the default
/// is to ignore them.
#ifndef ABSTRACT_TYPE
#define ABSTRACT_TYPE(Id, Parent)
#endif

#ifndef ABSTRACT_SUGARED_TYPE
#define ABSTRACT_SUGARED_TYPE(Id, Parent) ABSTRACT_TYPE(Id, Parent)
#endif

/// A convenience for determining the range of types.  These will always
/// appear immediately after the last member.
#ifndef TYPE_RANGE
#define TYPE_RANGE(Id, First, Last)
#endif

#ifndef LAST_TYPE
#define LAST_TYPE(Id)
#endif

#ifndef TYPE
# define TYPE(id, parent)
#endif

TYPE(Error, Type)
UNCHECKED_TYPE(Unresolved, Type)
ABSTRACT_TYPE(Builtin, Type)
  ABSTRACT_TYPE(AnyBuiltinInteger, BuiltinType)
    BUILTIN_TYPE(BuiltinInteger, AnyBuiltinIntegerType)
    BUILTIN_TYPE(BuiltinIntegerLiteral, AnyBuiltinIntegerType)
    TYPE_RANGE(AnyBuiltinInteger, BuiltinInteger, BuiltinIntegerLiteral)
  BUILTIN_TYPE(BuiltinFloat, BuiltinType)
  BUILTIN_TYPE(BuiltinRawPointer, BuiltinType)
  BUILTIN_TYPE(BuiltinNativeObject, BuiltinType)
  BUILTIN_TYPE(BuiltinBridgeObject, BuiltinType)
  BUILTIN_TYPE(BuiltinUnknownObject, BuiltinType)
  BUILTIN_TYPE(BuiltinUnsafeValueBuffer, BuiltinType)
  BUILTIN_TYPE(BuiltinVector, BuiltinType)
  TYPE_RANGE(Builtin, BuiltinInteger, BuiltinVector)
TYPE(Tuple, Type)

ABSTRACT_TYPE(AnyGeneric, Type)
  ABSTRACT_TYPE(NominalOrBoundGenericNominal, Type)
    ABSTRACT_TYPE(Nominal, Type)
      TYPE(Enum, NominalType)
      TYPE(Struct, NominalType)
      TYPE(Class, NominalType)
      TYPE(Protocol, NominalType)
      TYPE_RANGE(Nominal, Enum, Protocol)
    ABSTRACT_TYPE(BoundGeneric, Type)
      TYPE(BoundGenericClass, BoundGenericType)
      TYPE(BoundGenericEnum, BoundGenericType)
      TYPE(BoundGenericStruct, BoundGenericType)
      TYPE_RANGE(BoundGeneric, BoundGenericClass, BoundGenericStruct)
    TYPE_RANGE(NominalOrBoundGenericNominal, Enum, BoundGenericStruct)
  UNCHECKED_TYPE(UnboundGeneric, Type)
  TYPE_RANGE(AnyGeneric, Enum, UnboundGeneric)
ABSTRACT_TYPE(AnyMetatype, Type)
  TYPE(Metatype, AnyMetatypeType)
  TYPE(ExistentialMetatype, AnyMetatypeType)
ALWAYS_CANONICAL_TYPE(Module, Type)
TYPE(DynamicSelf, Type)
ABSTRACT_TYPE(Substitutable, Type)
  ABSTRACT_TYPE(Archetype, SubstitutableType)
    ALWAYS_CANONICAL_TYPE(PrimaryArchetype, ArchetypeType)
    ALWAYS_CANONICAL_TYPE(OpenedArchetype, ArchetypeType)
    ALWAYS_CANONICAL_TYPE(NestedArchetype, ArchetypeType)
    TYPE_RANGE(Archetype, PrimaryArchetype, NestedArchetype)
  TYPE(GenericTypeParam, SubstitutableType)
  TYPE_RANGE(Substitutable, PrimaryArchetype, GenericTypeParam)
TYPE(DependentMember, Type)
ABSTRACT_TYPE(AnyFunction, Type)
  TYPE(Function, AnyFunctionType)
  TYPE(GenericFunction, AnyFunctionType)
  TYPE_RANGE(AnyFunction, Function, GenericFunction)
ARTIFICIAL_TYPE(SILFunction, Type)
ARTIFICIAL_TYPE(SILBlockStorage, Type)
ARTIFICIAL_TYPE(SILBox, Type)
ARTIFICIAL_TYPE(SILToken, Type)
TYPE(ProtocolComposition, Type)
TYPE(LValue, Type)
TYPE(InOut, Type)
UNCHECKED_TYPE(TypeVariable, Type)
ABSTRACT_SUGARED_TYPE(Sugar, Type)
  SUGARED_TYPE(Paren, SugarType)
  SUGARED_TYPE(TypeAlias, SugarType)
  ABSTRACT_SUGARED_TYPE(SyntaxSugar, SugarType)
    ABSTRACT_SUGARED_TYPE(UnarySyntaxSugar, SyntaxSugarType)
      SUGARED_TYPE(ArraySlice, UnarySyntaxSugarType)
      SUGARED_TYPE(Optional, UnarySyntaxSugarType)
      TYPE_RANGE(UnarySyntaxSugar, ArraySlice, Optional)
    SUGARED_TYPE(Dictionary, SyntaxSugarType)
    TYPE_RANGE(SyntaxSugar, ArraySlice, Dictionary)
  TYPE_RANGE(Sugar, Paren, Dictionary)
LAST_TYPE(Dictionary) // Sugared types are last to make isa<SugarType>() fast.

#undef TYPE_RANGE
#undef ABSTRACT_SUGARED_TYPE
#undef ABSTRACT_TYPE
#undef UNCHECKED_TYPE
#undef ARTIFICIAL_TYPE
#undef SUGARED_TYPE
#undef BUILTIN_TYPE
#undef ALWAYS_CANONICAL_TYPE
#undef TYPE
#undef LAST_TYPE
