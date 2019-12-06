//===--- Semantics.def - Semantics Attribute Definitions -------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2019 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This macro is used to map the global definition name of a semantic
/// attribute to its raw value.
///   NAME: the global name used in the compiler
///   C_STR: the raw value used in swift
///
/// SEMANTICS_ATTR(NAME, C_STR)
///
//===----------------------------------------------------------------------===//

#ifndef SEMANTICS_ATTR
#error SEMANTICS_ATTR not defined.
#endif

SEMANTICS_ATTR(STRING_EQUALS, "string.equals")
SEMANTICS_ATTR(STRING_MAKE_UTF8, "string.makeUTF8")
SEMANTICS_ATTR(STRING_ESCAPE_PERCENT_GET, "string.escapePercent.get")
SEMANTICS_ATTR(STRING_CONCAT, "string.concat")
SEMANTICS_ATTR(STRING_APPEND, "string.append")
SEMANTICS_ATTR(STRING_INIT_EMPTY, "string.init_empty")
SEMANTICS_ATTR(STRING_PLUS_EQUALS, "string.plusequals")
SEMANTICS_ATTR(FIND_STRING_SWITCH_CASE, "findStringSwitchCase")
SEMANTICS_ATTR(FIND_STRING_SWITCH_CASE_WITH_CACHE, "findStringSwitchCaseWithCache")

SEMANTICS_ATTR(SWIFT_CONCURRENT_ASYNC, "swift.concurrent.async")
SEMANTICS_ATTR(SWIFT_CONCURRENT_SAFE, "swift.concurrent.safe")
SEMANTICS_ATTR(SWIFT_CONCURRENT, "swift.concurrent")

SEMANTICS_ATTR(ARRAY_APPEND_CONTENTS_OF, "array.append_contentsOf")
SEMANTICS_ATTR(ARRAY_APPEND_ELEMENT, "array.append_element")
SEMANTICS_ATTR(ARRAY_CHECK_INDEX, "array.check_index")
SEMANTICS_ATTR(ARRAY_CHECK_SUBSCRIPT, "array.check_subscript")
SEMANTICS_ATTR(ARRAY_GET_CAPACITY, "array.get_capacity")
SEMANTICS_ATTR(ARRAY_GET_COUNT, "array.get_count")
SEMANTICS_ATTR(ARRAY_GET_ELEMENT, "array.get_element")
SEMANTICS_ATTR(ARRAY_GET_ELEMENT_ADDRESS, "array.get_element_address")
SEMANTICS_ATTR(ARRAY_INIT, "array.init")
SEMANTICS_ATTR(ARRAY_INIT_EMPTY, "array.init.empty")
SEMANTICS_ATTR(ARRAY_MAKE_MUTABLE, "array.make_mutable")
SEMANTICS_ATTR(ARRAY_MUTATE_UNKNOWN, "array.mutate_unknown")
SEMANTICS_ATTR(ARRAY_PROPS_IS_NATIVE_TYPE_CHECKED, "array.props.isNativeTypeChecked")
SEMANTICS_ATTR(ARRAY_RESERVE_CAPACITY_FOR_APPEND, "array.reserve_capacity_for_append")
SEMANTICS_ATTR(ARRAY_UNINITIALIZED, "array.uninitialized")
SEMANTICS_ATTR(ARRAY_WITH_UNSAFE_MUTABLE_BUFFER_POINTER, "array.withUnsafeMutableBufferPointer")
SEMANTICS_ATTR(ARRAY_COUNT, "array.count")
SEMANTICS_ATTR(ARRAY_DEALLOC_UNINITIALIZED, "array.dealloc_uninitialized")
SEMANTICS_ATTR(ARRAY_UNINITIALIZED_INTRINSIC, "array.uninitialized_intrinsic")

SEMANTICS_ATTR(OPTIMIZE_SIL_SPECIALIZE_GENERIC_NEVER, "optimize.sil.specialize.generic.never")
SEMANTICS_ATTR(OPTIMIZE_SIL_SPECIALIZE_GENERIC_PARTIAL_NEVER,
          "optimize.sil.specialize.generic.partial.never")
SEMANTICS_ATTR(OPTIMIZE_SIL_SPECIALIZE_GENERIC_SIZE_NEVER,
          "optimize.sil.specialize.generic.size.never")

SEMANTICS_ATTR(OSLOG_INTERPOLATION_INIT, "oslog.interpolation.init")
SEMANTICS_ATTR(OSLOG_MESSAGE_INIT_INTERPOLATION, "oslog.message.init_interpolation")
SEMANTICS_ATTR(OSLOG_MESSAGE_INIT_STRING_LITERAL, "oslog.message.init_stringliteral")

SEMANTICS_ATTR(TYPE_CHECKER_OPEN_EXISTENTIAL, "typechecker._openExistential(_:do:)")
SEMANTICS_ATTR(TYPE_CHECKER_TYPE, "typechecker.type(of:)")
SEMANTICS_ATTR(TYPE_CHECKER_WITHOUT_ACTUALLY_ESCAPING, "typechecker.withoutActuallyEscaping(_:do:)")

SEMANTICS_ATTR(AVAILABILITY_OSVERSION, "availability.osversion")

SEMANTICS_ATTR(CONSTANT_EVALUABLE, "constant_evaluable")
SEMANTICS_ATTR(EXIT, "exit")
SEMANTICS_ATTR(FASTPATH, "fastpath")
SEMANTICS_ATTR(SLOWPATH, "slowpath")
SEMANTICS_ATTR(PROGRAMTERMINATION_POINT, "programtermination_point")
SEMANTICS_ATTR(CONVERT_TO_OBJECTIVE_C, "convertToObjectiveC")

#undef SEMANTICS_ATTR

