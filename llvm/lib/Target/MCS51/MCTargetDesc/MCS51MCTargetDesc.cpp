//===-- MCS51MCTargetDesc.cpp - MCS51 Target Descriptions -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// This file provides MCS51-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "TargetInfo/MCS51TargetInfo.h"
#include "MCS51MCTargetDesc.h"
#include "MCS51MCAsmInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "MCS51GenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MCS51GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createMCS51MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitMCS51MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createMCS51MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitMCS51MCRegisterInfo(X, MCS51::A);
  return X;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMCS51TargetMC() {
  Target &T = getTheMCS51Target();
  RegisterMCAsmInfo<MCS51MCAsmInfo> X(T);
  TargetRegistry::RegisterMCInstrInfo(T, createMCS51MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createMCS51MCRegisterInfo);
  TargetRegistry::RegisterMCAsmBackend(T, createMCS51AsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(T, createMCS51MCCodeEmitter);
}
