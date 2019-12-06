//===------------- IncrementalRanges.h -----------------------------*- C++-*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2018 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef POLARPHP_AST_INCREMENTALRANGES_H
#define POLARPHP_AST_INCREMENTALRANGES_H

// These are the declarations for managing serializable source locations so that
// the frontend and the driver can implement incremental compilation based on
// source ranges.

#include "polarphp/basic/LLVM.h"
#include "polarphp/basic/NullablePtr.h"
#include "polarphp/basic/SourceLoc.h"
#include "polarphp/basic/StringExtras.h"
#include "llvm/Support/YAMLTraits.h"
#include <vector>

namespace polar::basic {
class SourceManager;
} // namespace polar::basic

//==============================================================================
// MARK: Range types
//==============================================================================

namespace polar::ast {
class DiagnosticEngine;
class SourceFile;
using polar::basic::CharSourceRange;
using polar::basic::SourceManager;
using polar::basic::SourceLoc;

namespace incremental_ranges {

struct SerializableSourceRange;

typedef std::vector<SerializableSourceRange> Ranges;
typedef std::map<std::string, Ranges> RangesByFilename;
} // namespace incremental_ranges
} // namespace polar::ast

//==============================================================================
// MARK: SerializableSourceLocation
//==============================================================================
namespace polar::ast {
namespace incremental_ranges {

/// A source location that can be written from the frontend and read by the
/// driver. 1-origin: lines and columns start at 1
struct SerializableSourceLocation {
   uint64_t line = 0;
   uint64_t column = 0;

   SerializableSourceLocation(const SourceLoc loc, const SourceManager &SM);
   SerializableSourceLocation(uint64_t line, uint64_t column)
      : line(line), column(column) {}
   SerializableSourceLocation() = default;
   static const SerializableSourceLocation endOfAnyFile;

   bool operator< (const SerializableSourceLocation &x) const {
      return line < x.line ? true
                           : line > x.line ? false
                                           : column < x.column;
   }
   bool operator==(const SerializableSourceLocation &x) const {
      return line == x.line && column == x.column;
   }
   bool operator<=(const SerializableSourceLocation &x) const {
      return *this < x || *this == x;
   }
   void print(raw_ostream &out) const;
   void dump() const;
};

} // namespace incremental_ranges
} // namespace polar::ast

template <>
struct llvm::yaml::MappingTraits<
      polar::ast::incremental_ranges::SerializableSourceLocation> {
   static const bool flow = true;
   static void
   mapping(llvm::yaml::IO &io,
           polar::ast::incremental_ranges::SerializableSourceLocation &loc) {
      io.mapRequired("line", loc.line), io.mapRequired("column", loc.column);
   }
};
//==============================================================================
// MARK: SerializableSourceRange
//==============================================================================

namespace polar::ast {
namespace incremental_ranges {
/// A range in the source, that can be written by the frontend and read by the
/// driver. Half-open, to facilitate representing empty ranges. In other words,
/// an empty region will have start == end
struct SerializableSourceRange {
   SerializableSourceLocation start, end;

   SerializableSourceRange(const CharSourceRange r, const SourceManager &SM);
   SerializableSourceRange(SerializableSourceLocation start,
                           SerializableSourceLocation end);
   SerializableSourceRange() = default;

   static const SerializableSourceRange wholeFile;
   static Ranges RangesForWholeFile();

   bool isEmpty() const { return start == end; }

   bool overlaps(const SerializableSourceRange &x) const {
      return start < x.end && x.start < end;
   }
   bool operator==(const SerializableSourceRange &x) const {
      return start == x.start && end == x.end;
   }
   bool isImproperSubsetOf(const SerializableSourceRange &) const;
   bool properlyPreceeds(const SerializableSourceRange &) const;
   static bool isProperlySorted(ArrayRef<SerializableSourceRange>);

   bool
   isImproperSubsetOfAny(ArrayRef<SerializableSourceRange> supersetRanges) const;
   bool isImproperSubsetOfAnySlowlyAndSimply(
         ArrayRef<SerializableSourceRange> supersetRanges) const;

   /// Optimized for fewer ranges in the subset
   /// Return first outlier found in subset not in superset
   static Optional<SerializableSourceRange>
   findOutlierIfAny(ArrayRef<SerializableSourceRange> subset,
                    ArrayRef<SerializableSourceRange> superset);

   static Ranges findAllOutliers(ArrayRef<SerializableSourceRange> subset,
                                 ArrayRef<SerializableSourceRange> superset);

   std::string printString() const;
   void print(raw_ostream &out) const;
   void dump() const;
};

} // namespace incremental_ranges
} // namespace polar::ast


template <>
struct llvm::yaml::MappingTraits<
      polar::ast::incremental_ranges::SerializableSourceRange> {
   static const bool flow = true;
   static void mapping(llvm::yaml::IO &io,
                       polar::ast::incremental_ranges::SerializableSourceRange &sr) {
      io.mapRequired("start", sr.start);
      io.mapRequired("end", sr.end);
   }
};

//==============================================================================
// MARK: PolarphpRangesFileContents
//==============================================================================

namespace polar::ast {
namespace incremental_ranges {

/// The complete contents of the file written by the frontend and read by the
/// driver containing source range information for one primary input file.
struct PolarphpRangesFileContents {
   /// For each non-primary, the unparsed ranges in it.
   /// At present these represent the bodies of types defined in the nonprimary
   /// that are not used in the primary.
   Ranges noninlinableFunctionBodies;

   PolarphpRangesFileContents() = default;

   PolarphpRangesFileContents(Ranges &&noninlinableFunctionBodies)
      : noninlinableFunctionBodies(std::move(noninlinableFunctionBodies)) {}

   /// Return None for error.
   static Optional<PolarphpRangesFileContents>
   load(const StringRef primaryPath, const llvm::MemoryBuffer &swiftRangesBuffer,
        const bool showIncrementalBuildDecisions, DiagnosticEngine &diags);

   void dump(StringRef primaryFilename) const;

   static constexpr const char *header = "### Swift source ranges file v0 ###\n";
};
} // namespace incremental_ranges
} // namespace polar::ast

template <>
struct llvm::yaml::MappingTraits<
      polar::ast::incremental_ranges::PolarphpRangesFileContents> {
   static void
   mapping(llvm::yaml::IO &io,
           polar::ast::incremental_ranges::PolarphpRangesFileContents &srfc) {
      io.mapRequired("noninlinableFunctionBodies",
                     srfc.noninlinableFunctionBodies);
   }
};

LLVM_YAML_IS_SEQUENCE_VECTOR(polar::ast::incremental_ranges::SerializableSourceRange);
LLVM_YAML_IS_STRING_MAP(polar::ast::incremental_ranges::Ranges);
LLVM_YAML_IS_STRING_MAP(polar::ast::incremental_ranges::RangesByFilename);

//==============================================================================
// MARK: PolarphpRangesEmitter
//==============================================================================
namespace polar::ast {
namespace incremental_ranges {
/// Gathers up the information from the frontend, processes it, and writes it.
class PolarphpRangesEmitter {
   const StringRef outputPath;
   SourceFile *const primaryFile;
   const SourceManager &sourceMgr;
   DiagnosticEngine &diags;

public:
   PolarphpRangesEmitter(StringRef outputPath, SourceFile *primaryFile,
                         const SourceManager &sourceMgr, DiagnosticEngine &diags)
      : outputPath(outputPath), primaryFile(primaryFile), sourceMgr(sourceMgr),
        diags(diags) {}

   /// True for error
   bool emit() const;

public:
   void emitRanges(llvm::raw_ostream &out) const;

private:
   Ranges collectSortedSerializedNoninlinableFunctionBodies() const;
   std::vector<CharSourceRange> collectNoninlinableFunctionBodies() const;

   std::vector<CharSourceRange>
   sortRanges(std::vector<CharSourceRange> ranges) const;

   /// Assuming \p ranges is sorted, coalesce overlapping ranges in place and
   /// return end of the resultant vector.
   std::vector<CharSourceRange>
   coalesceSortedRanges(std::vector<CharSourceRange>) const;

   std::vector<SerializableSourceRange>
   serializeRanges(std::vector<CharSourceRange> ranges) const;

   bool isImmediatelyBeforeOrOverlapping(CharSourceRange prev,
                                         CharSourceRange next) const;
};
} // namespace incremental_ranges
} // namespace polar::ast

//==============================================================================
// MARK: CompiledSourceEmitter
//==============================================================================
namespace polar::ast {
namespace incremental_ranges {
/// The class that writes out the unchanged source code in the primary input so
/// that the driver can diff it later, after the user has changed the file.
class CompiledSourceEmitter
{
   const StringRef outputPath;
   const SourceFile *const primaryFile;
   const SourceManager &sourceMgr;
   DiagnosticEngine &diags;

public:
   CompiledSourceEmitter(StringRef outputPath, const SourceFile *primaryFile,
                         const SourceManager &sourceMgr, DiagnosticEngine &diags)
      : outputPath(outputPath), primaryFile(primaryFile), sourceMgr(sourceMgr),
        diags(diags) {}

   /// True for error
   bool emit();
};

} // namespace incremental_ranges
} // namespace polar::ast

#endif // POLARPHP_AST_INCREMENTALRANGES_H
