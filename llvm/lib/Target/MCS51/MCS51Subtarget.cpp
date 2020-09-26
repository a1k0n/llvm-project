//===-- MCS51Subtarget.cpp - MCS51 Subtarget Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MCS51 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MCS51Subtarget.h"
#include "MCS51.h"
#include "MCS51FrameLowering.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "MCS51-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MCS51GenSubtargetInfo.inc"

void MCS51Subtarget::anchor() {}

MCS51Subtarget &MCS51Subtarget::initializeSubtargetDependencies(
    StringRef CPU, StringRef TuneCPU, StringRef FS) {
  // Determine default and user-specified characteristics
  StringRef CPUName = CPU;
  if (CPUName.empty())
    CPUName = "generic-8051";
  ParseSubtargetFeatures(CPUName, TuneCPU, FS);
  return *this;
}

MCS51Subtarget::MCS51Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               const TargetMachine &TM)
    : MCS51GenSubtargetInfo(TT, CPU, /*TuneCPU*/StringRef(), FS),
      FrameLowering(initializeSubtargetDependencies(CPU, /*TuneCPU*/StringRef(), FS)),
      InstrInfo(), RegInfo(getHwMode()), TLInfo(TM, *this) {}
