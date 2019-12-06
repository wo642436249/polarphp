//===--- PlatformKinds.def - Swift PlatformKind Metaprogramming -*- C++ -*-===//
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
// This file defines macros used for macro-metaprogramming with platform kinds.
//
//===----------------------------------------------------------------------===//

/// AVAILABILITY_PLATFORM(X, PrettyName)
///   X - The name of the platform
///   PrettyName - A string with the platform name for pretty printing
#ifndef AVAILABILITY_PLATFORM
#define AVAILABILITY_PLATFORM(X, PrettyName)
#endif

// Reordering these platforms will break serialization.
AVAILABILITY_PLATFORM(iOS, "iOS")
AVAILABILITY_PLATFORM(tvOS, "tvOS")
AVAILABILITY_PLATFORM(watchOS, "watchOS")
AVAILABILITY_PLATFORM(OSX, "macOS")
AVAILABILITY_PLATFORM(iOSApplicationExtension, "application extensions for iOS")
AVAILABILITY_PLATFORM(tvOSApplicationExtension, "application extensions for tvOS")
AVAILABILITY_PLATFORM(watchOSApplicationExtension, "application extensions for watchOS")
AVAILABILITY_PLATFORM(OSXApplicationExtension, "application extensions for macOS")

#undef AVAILABILITY_PLATFORM
