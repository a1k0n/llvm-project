//===-- MCS51TargetInfo.cpp - MCS51 Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace llvm {
Target &getTheMCS51Target() {
  static Target TheMCS51Target;
  return TheMCS51Target;
}
}

extern "C" void LLVMInitializeMCS51TargetInfo() {
  RegisterTarget<Triple::mcs51> X(getTheMCS51Target(), "mcs51", "MCS51/8051 Family", "MCS51");
}

// FIXME: Temporary stub - this function must be defined for linking
// to succeed and will be called unconditionally by llc, so must be a no-op.
// Remove once this function is properly implemented.
extern "C" void LLVMInitializeMCS51TargetMC() {}
