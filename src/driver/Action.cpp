//===--- Action.cpp - Abstract compilation steps --------------------------===//
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

#include "polarphp/driver/Action.h"

#include "llvm/Support/ErrorHandling.h"

using namespace polar::driver;
using namespace llvm::opt;

const char *Action::getClassName(Kind AC) {
   switch (AC) {
      case Kind::Input:  return "input";
      case Kind::CompileJob:  return "compile";
      case Kind::InterpretJob:  return "interpret";
      case Kind::BackendJob:  return "backend";
      case Kind::MergeModuleJob:  return "merge-module";
      case Kind::ModuleWrapJob:  return "modulewrap";
      case Kind::AutolinkExtractJob:  return "swift-autolink-extract";
      case Kind::REPLJob:  return "repl";
      case Kind::DynamicLinkJob:  return "link";
      case Kind::StaticLinkJob:  return "static-link";
      case Kind::GenerateDSYMJob:  return "generate-dSYM";
      case Kind::VerifyDebugInfoJob:  return "verify-debug-info";
      case Kind::GeneratePCHJob:  return "generate-pch";
   }

   llvm_unreachable("invalid class");
}

void InputAction::anchor() {}

void JobAction::anchor() {}

void CompileJobAction::anchor() {}

void InterpretJobAction::anchor() {}

void BackendJobAction::anchor() {}

void MergeModuleJobAction::anchor() {}

void ModuleWrapJobAction::anchor() {}

void AutolinkExtractJobAction::anchor() {}

void REPLJobAction::anchor() {}

void DynamicLinkJobAction::anchor() {}

void StaticLinkJobAction::anchor() {}

void GenerateDSYMJobAction::anchor() {}

void VerifyDebugInfoJobAction::anchor() {}

void GeneratePCHJobAction::anchor() {}
