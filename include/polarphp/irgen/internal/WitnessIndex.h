//===--- WitnessIndex.h - Index into a witness table ------------*- C++ -*-===//
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
// This file defines the WitnessIndex type, used for drilling into a
// protocol witness table or value witness table.
//
//===----------------------------------------------------------------------===//

#ifndef POLARPHP_IRGEN_INTERNAL_WITNESSINDEX_H
#define POLARPHP_IRGEN_INTERNAL_WITNESSINDEX_H

#include "polarphp/abi/MetadataValues.h"
#include "polarphp/irgen/ValueWitness.h"

namespace polar::irgen {

/// A class which encapsulates an index into a witness table.
class WitnessIndex {
   // Negative values are indexing into the private area of a protocol witness
   // table.
   int Value : 31;
   unsigned IsPrefix : 1;
public:
   WitnessIndex() = default;
   explicit WitnessIndex(int index, bool isPrefix)
      : Value(index), IsPrefix(isPrefix) {}

   int getValue() const { return Value; }

   bool isPrefix() const { return IsPrefix; }

   /// Adjust the index to refer into a protocol witness table (rather than
   /// a value witness table).
   WitnessIndex forInterfaceWitnessTable() const {
      int NewValue = Value < 0
                     ? Value
                     : Value + WitnessTableFirstRequirementOffset;
      return WitnessIndex(NewValue, IsPrefix);
   }
};

} // polar::irgen

#endif // POLARPHP_IRGEN_INTERNAL_WITNESSINDEX_H
