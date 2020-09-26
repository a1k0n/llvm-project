//===-- MCS51RegisterInfo.cpp - MCS51 Register Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MCS51 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "MCS51RegisterInfo.h"
#include "MCS51.h"
#include "MCS51Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "MCS51GenRegisterInfo.inc"

using namespace llvm;

MCS51RegisterInfo::MCS51RegisterInfo(unsigned HwMode)
    : MCS51GenRegisterInfo(MCS51::SP, /*DwarfFlavour*/0, /*EHFlavor*/0,
                           /*PC*/0, HwMode) {}

const MCPhysReg *
MCS51RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

BitVector MCS51RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  // Use markSuperRegs to ensure any register aliases are also reserved
  markSuperRegs(Reserved, MCS51::SP);
  // markSuperRegs(Reserved, MCS51::DPTR); ??
  assert(checkAllSuperRegsMarked(Reserved));
  return Reserved;
}

void MCS51RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  report_fatal_error("Subroutines not supported yet");
}

Register MCS51RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  // no frame pointer
  return MCS51::NoRegister;
  // return MCS51::SP;
}

