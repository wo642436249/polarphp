//===--- KnownStdlibTypes.def - Common standard library types ---*- C++ -*-===//
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
//  This xmacro generates code for common standard library types the compiler
//  has special knowledge of.
//
//===----------------------------------------------------------------------===//

#ifndef KNOWN_STDLIB_TYPE_DECL
/// KNOWN_STDLIB_TYPE_DECL(NAME, DECL_CLASS, NUM_GENERIC_PARAMS)
///
/// The macro is expanded for each known standard library type. NAME is
/// bound to the unqualified name of the type. DECL_CLASS is bound to the
/// Decl subclass it is expected to be an instance of. NUM_GENERIC_PARAMS is
/// bound to the number of generic parameters the type is expected to have.
#define KNOWN_STDLIB_TYPE_DECL(NAME, DECL_CLASS, NUM_GENERIC_PARAMS)
#endif

KNOWN_STDLIB_TYPE_DECL(Void, TypeAliasDecl, 0)

KNOWN_STDLIB_TYPE_DECL(Bool, NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(Int,   NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Int64, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Int32, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Int16, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Int8,  NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(UInt,   NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UInt64, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UInt32, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UInt16, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UInt8,  NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(Float,  NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Double, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Float80, NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(_MaxBuiltinFloatType, TypeAliasDecl, 0)

KNOWN_STDLIB_TYPE_DECL(String, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(StaticString, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Substring, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(Array, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(Set, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(Sequence, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(Dictionary, NominalTypeDecl, 2)
KNOWN_STDLIB_TYPE_DECL(CollectionDifference, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(AnyHashable, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(MutableCollection, InterfaceDecl, 1)
KNOWN_STDLIB_TYPE_DECL(Hasher, NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(AnyKeyPath, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(PartialKeyPath, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(KeyPath, NominalTypeDecl, 2)
KNOWN_STDLIB_TYPE_DECL(WritableKeyPath, NominalTypeDecl, 2)
KNOWN_STDLIB_TYPE_DECL(ReferenceWritableKeyPath, NominalTypeDecl, 2)

KNOWN_STDLIB_TYPE_DECL(Optional, EnumDecl, 1)

KNOWN_STDLIB_TYPE_DECL(OptionSet, NominalTypeDecl, 1)

KNOWN_STDLIB_TYPE_DECL(UnsafeMutableRawPointer, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UnsafeRawPointer, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UnsafeMutablePointer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(UnsafePointer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(OpaquePointer, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(AutoreleasingUnsafeMutablePointer, NominalTypeDecl, 1)

KNOWN_STDLIB_TYPE_DECL(UnsafeBufferPointer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(UnsafeMutableBufferPointer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(UnsafeRawBufferPointer, NominalTypeDecl, 0)
KNOWN_STDLIB_TYPE_DECL(UnsafeMutableRawBufferPointer, NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(Unmanaged, NominalTypeDecl, 1)

KNOWN_STDLIB_TYPE_DECL(Never, NominalTypeDecl, 0)

KNOWN_STDLIB_TYPE_DECL(Encoder, InterfaceDecl, 1)
KNOWN_STDLIB_TYPE_DECL(Decoder, InterfaceDecl, 1)
KNOWN_STDLIB_TYPE_DECL(KeyedEncodingContainer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(KeyedDecodingContainer, NominalTypeDecl, 1)
KNOWN_STDLIB_TYPE_DECL(RangeReplaceableCollection, InterfaceDecl, 1)

#undef KNOWN_STDLIB_TYPE_DECL
