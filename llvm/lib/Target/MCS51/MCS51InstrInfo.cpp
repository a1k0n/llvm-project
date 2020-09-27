//===-- MCS51InstrInfo.cpp - MCS51 Instruction Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MCS51 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MCS51InstrInfo.h"
#include "MCS51.h"
#include "MCS51Subtarget.h"
#include "MCS51TargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "MCS51GenInstrInfo.inc"

using namespace llvm;

MCS51InstrInfo::MCS51InstrInfo() : MCS51GenInstrInfo() {}

void MCS51InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL, MCRegister DstReg,
                                 MCRegister SrcReg, bool KillSrc) const {
  if (DstReg == MCS51::ACC) {
    if (MCS51::GPRnRegClass.contains(SrcReg)) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVARn))
          .addReg(SrcReg, getKillRegState(KillSrc));
    } else {
      // FIXME
      llvm_unreachable("trying to move non-Rn into A unimplemented");
    }
  } else if (SrcReg == MCS51::ACC) {
    if (MCS51::GPRnRegClass.contains(DstReg)) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVRnA), DstReg);
    } else {
      llvm_unreachable("trying to move non-Rn into A unimplemented");
    }
  } else if (MCS51::GPRnRegClass.contains(SrcReg, DstReg)) {
    BuildMI(MBB, MBBI, DL, get(MCS51::MOVRnRn), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
  }
}