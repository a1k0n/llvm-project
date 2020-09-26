//===-- MCS51.h - Top-level interface for MCS51 -----------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// RISC-V back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51_H
#define LLVM_LIB_TARGET_MCS51_MCS51_H

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class MCS51TargetMachine;
class MCInst;
class MachineInstr;

void LowerMCS51MachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI);

FunctionPass *createMCS51ISelDag(MCS51TargetMachine &TM);
}

#endif

