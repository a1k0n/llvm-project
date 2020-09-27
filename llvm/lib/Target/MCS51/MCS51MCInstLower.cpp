//===-- MCS51MCInstLower.cpp - Convert MCS51 MachineInstr to an MCInst ------=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower MCS51 MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "MCS51.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mcs51-lower-mcinst"

bool llvm::LowerMCS51MachineOperandToMCOperand(const MachineOperand &MO,
                                               MCOperand &MCOp) {
  switch (MO.getType()) {
  default:
    report_fatal_error("LowerMCS51MachineInstrToMCInst: unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit()) {
      LLVM_DEBUG(dbgs() << "implicit register operand "; MO.dump());
      return true;
    }
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_Immediate:
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  }
  return true;
}

void llvm::LowerMCS51MachineInstrToMCInst(const MachineInstr *MI,
                                          MCInst &OutMI) {
  OutMI.setOpcode(MI->getOpcode());

  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    LowerMCS51MachineOperandToMCOperand(MO, MCOp);
    OutMI.addOperand(MCOp);
  }
}
