//===--- TrailingCallArguments.h - Trailing Call Arguments ------*- C++ -*-===//
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
// This source file is part of the polarphp.org open source project
// Copyright (c) 2017 - 2019 polarphp software foundation
// Copyright (c) 2017 - 2019 zzu_softboy <zzu_softboy@163.com>
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://polarphp.org/LICENSE.txt for license information
// See https://polarphp.org/CONTRIBUTORS.txt for the list of polarphp project authors
//
// Created by polarboy on 2019/11/27.
//
//===----------------------------------------------------------------------===//

//
//  This file defines the TrailingCallArguments template, which is used
//  to tail-allocate the names and source locations of argument labels in a
//  call.
//
//===----------------------------------------------------------------------===//

#ifndef POLARPHP_AST_TRAILING_CALL_ARGUMENTS_H
#define POLARPHP_AST_TRAILING_CALL_ARGUMENTS_H

#include "polarphp/syntax/TokenSyntax.h"
#include "polarphp/basic/SourceLoc.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/TrailingObjects.h"

namespace polar::ast {

using polar::syntax::TokenSyntax;
using polar::basic::SourceLoc;
using polar::basic::MutableArrayRef;
using polar::basic::ArrayRef;

/// Helper class to capture trailing call argument labels and related
/// information, for expression nodes that involve argument labels, trailing
/// closures, etc.
template<typename Derived>
class TrailingCallArguments
      : private llvm::TrailingObjects<Derived, TokenSyntax, SourceLoc>
{
   // We need to friend TrailingObjects twice here to work around an MSVC bug.
   // If we have two functions of the same name with the parameter
   // typename TrailingObjectsIdentifier::template OverloadToken<T> where T is
   // different for each function, then MSVC reports a "member function already
   // defined or declared" error, which is incorrect.
   using TrailingObjectsIdentifier = llvm::TrailingObjects<Derived, TokenSyntax>;
   friend TrailingObjectsIdentifier;

   using TrailingObjects = llvm::TrailingObjects<Derived, TokenSyntax, SourceLoc>;
   friend TrailingObjects;

   Derived &asDerived()
   {
      return *static_cast<Derived *>(this);
   }

   const Derived &asDerived() const
   {
      return *static_cast<const Derived *>(this);
   }

   size_t numTrailingObjects(
         typename TrailingObjectsIdentifier::template OverloadToken<TokenSyntax>) const
   {
      return asDerived().getNumArguments();
   }

   size_t numTrailingObjects(
         typename TrailingObjectsIdentifier::template OverloadToken<SourceLoc>) const
   {
      return asDerived().hasArgumentLabelLocs() ? asDerived().getNumArguments()
                                                : 0;
   }

   /// Retrieve the buffer containing the argument labels.
   MutableArrayRef<TokenSyntax> getArgumentLabelsBuffer()
   {
      return { this->template getTrailingObjects<TokenSyntax>(),
         asDerived().getNumArguments() };
   }

   /// Retrieve the buffer containing the argument label locations.
   MutableArrayRef<SourceLoc> getArgumentLabelLocsBuffer()
   {
      if (!asDerived().hasArgumentLabelLocs()) {
         return { };
      }
      return { this->template getTrailingObjects<SourceLoc>(),
         asDerived().getNumArguments() };
   }

protected:
   /// Determine the total size to allocate.
   static size_t totalSizeToAlloc(ArrayRef<TokenSyntax> argLabels,
                                  ArrayRef<SourceLoc> argLabelLocs,
                                  bool hasTrailingClosure)
   {
      return TrailingObjects::template totalSizeToAlloc<TokenSyntax, SourceLoc>(
               argLabels.size(), argLabelLocs.size());
   }

   /// Initialize the actual call arguments.
   void initializeCallArguments(ArrayRef<TokenSyntax> argLabels,
                                ArrayRef<SourceLoc> argLabelLocs,
                                bool hasTrailingClosure)
   {
      if (!argLabels.empty()) {
         std::uninitialized_copy(argLabels.begin(), argLabels.end(),
                                 this->template getTrailingObjects<TokenSyntax>());
      }

      if (!argLabelLocs.empty()) {
         std::uninitialized_copy(argLabelLocs.begin(), argLabelLocs.end(),
                                 this->template getTrailingObjects<SourceLoc>());
      }
   }

public:
   /// Retrieve the argument labels provided at the call site.
   ArrayRef<TokenSyntax> getArgumentLabels() const
   {
      return { this->template getTrailingObjects<TokenSyntax>(),
         asDerived().getNumArguments() };
   }

   /// Retrieve the buffer containing the argument label locations.
   ArrayRef<SourceLoc> getArgumentLabelLocs() const
   {
      if (!asDerived().hasArgumentLabelLocs()) {
         return {};
      }
      return { this->template getTrailingObjects<SourceLoc>(),
         asDerived().getNumArguments() };
   }

   /// Retrieve the location of the ith argument label.
   SourceLoc getArgumentLabelLoc(unsigned i) const
   {
      auto locs = getArgumentLabelLocs();
      return i < locs.size() ? locs[i] : SourceLoc();
   }
};

} // polar::ast

#endif // POLARPHP_AST_TRAILING_CALL_ARGUMENTS_H