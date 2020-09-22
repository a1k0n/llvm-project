//===-- MCS51TargetInfo.cpp - MCS51 Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

namespace llvm {
Target &getTheMCS51Target() {
  static Target TheMCS51Target;
  return TheMCS51Target;
}
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMCS51TargetInfo() {
  llvm::RegisterTarget<llvm::Triple::mcs51> X(llvm::getTheMCS51Target(),
      "mcs51", "MCS51/8051 Family", "MCS51");
}
