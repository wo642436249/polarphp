//===--- PlatformKind.cpp - Swift Language Platform Kinds -----------------===//
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
// This file implements the platform kinds for API availability.
//
//===----------------------------------------------------------------------===//

#include "polarphp/ast/PlatformKind.h"
#include "polarphp/kernel/LangOptions.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/ErrorHandling.h"

namespace polar::ast {


StringRef platformString(PlatformKind platform) {
  switch (platform) {
  case PlatformKind::none:
    return "*";
#define AVAILABILITY_PLATFORM(X, PrettyName)                                   \
  case PlatformKind::X:                                                        \
    return #X;
#include "polarphp/ast/PlatformKindsDef.h"
  }
  llvm_unreachable("bad PlatformKind");
}

StringRef prettyPlatformString(PlatformKind platform) {
  switch (platform) {
  case PlatformKind::none:
    return "*";
#define AVAILABILITY_PLATFORM(X, PrettyName)                                   \
  case PlatformKind::X:                                                        \
    return PrettyName;
#include "polarphp/ast/PlatformKindsDef.h"
  }
  llvm_unreachable("bad PlatformKind");
}

Optional<PlatformKind> platformFromString(StringRef Name) {
  if (Name == "*")
    return PlatformKind::none;
  return llvm::StringSwitch<Optional<PlatformKind>>(Name)
#define AVAILABILITY_PLATFORM(X, PrettyName) .Case(#X, PlatformKind::X)
#include "polarphp/ast/PlatformKindsDef.h"
      .Case("macOS", PlatformKind::OSX)
      .Case("macOSApplicationExtension", PlatformKind::OSXApplicationExtension)
      .Default(Optional<PlatformKind>());
}

static bool isPlatformActiveFortarget(PlatformKind Platform,
                                      const llvm::Triple &target,
                                      bool enableAppExtensionRestrictions) {
  if (Platform == PlatformKind::none)
    return true;

  if (Platform == PlatformKind::OSXApplicationExtension ||
      Platform == PlatformKind::iOSApplicationExtension)
    if (!enableAppExtensionRestrictions)
      return false;

  // FIXME: This is an awful way to get the current OS.
  switch (Platform) {
    case PlatformKind::OSX:
    case PlatformKind::OSXApplicationExtension:
      return target.isMacOSX();
    case PlatformKind::iOS:
    case PlatformKind::iOSApplicationExtension:
      return target.isiOS() && !target.isTvOS();
    case PlatformKind::tvOS:
    case PlatformKind::tvOSApplicationExtension:
      return target.isTvOS();
    case PlatformKind::watchOS:
    case PlatformKind::watchOSApplicationExtension:
      return target.isWatchOS();
    case PlatformKind::none:
      llvm_unreachable("handled above");
  }
  llvm_unreachable("bad PlatformKind");
}

bool isPlatformActive(PlatformKind Platform, LangOptions &LangOpts) {
  llvm::Triple TT = LangOpts.target;
  return isPlatformActiveFortarget(Platform, TT,
                                   LangOpts.enableAppExtensionRestrictions);
}

PlatformKind targetPlatform(LangOptions &LangOpts) {
  if (LangOpts.target.isMacOSX()) {
    return (LangOpts.enableAppExtensionRestrictions
                ? PlatformKind::OSXApplicationExtension
                : PlatformKind::OSX);
  }

  if (LangOpts.target.isTvOS()) {
    return (LangOpts.enableAppExtensionRestrictions
            ? PlatformKind::tvOSApplicationExtension
            : PlatformKind::tvOS);
  }

  if (LangOpts.target.isWatchOS()) {
    return (LangOpts.enableAppExtensionRestrictions
            ? PlatformKind::watchOSApplicationExtension
            : PlatformKind::watchOS);
  }

  if (LangOpts.target.isiOS()) {
    return (LangOpts.enableAppExtensionRestrictions
                ? PlatformKind::iOSApplicationExtension
                : PlatformKind::iOS);
  }

  return PlatformKind::none;
}

} // polar::ast