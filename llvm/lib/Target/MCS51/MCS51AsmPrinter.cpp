//===-- MCS51AsmPrinter.cpp - MCS51 LLVM assembly writer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the MCS51 assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCS51.h"
#include "MCS51TargetMachine.h"
#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
class MCS51AsmPrinter : public AsmPrinter {
public:
  explicit MCS51AsmPrinter(TargetMachine &TM,
                           std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  StringRef getPassName() const override { return "MCS51 Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;

  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);

  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp);
};
}

// Simple pseudo-instructions have their lowering (with expansion to real
// instructions) auto-generated.
#include "MCS51GenMCPseudoLowering.inc"

bool MCS51AsmPrinter::lowerOperand(const MachineOperand &MO, MCOperand &MCOp) {
  return LowerMCS51MachineOperandToMCOperand(MO, MCOp, *this);
}

void MCS51AsmPrinter::emitInstruction(const MachineInstr *MI) {
  // Do any auto-generated pseudo lowerings.
  if (emitPseudoExpansionLowering(*OutStreamer, MI))
    return;

  // custom lowering for MOVAnyDirect (load) and MOVDirectAny (store)
  switch (MI->getOpcode()) {
  default:
    break;
  case MCS51::MOVAnyImm: {
    Register Rd = MI->getOperand(0).getReg();
    MCInst MovInst;
    MCOperand MCSrc;
    if (Rd == MCS51::ACC) {
      MovInst.setOpcode(MCS51::MOVAImm8);
    } else {
      MovInst.setOpcode(MCS51::MOVRnImm8);
      MCOperand MCDst;
      lowerOperand(MI->getOperand(0), MCDst);
      MovInst.addOperand(MCDst);
    }
    lowerOperand(MI->getOperand(1), MCSrc);
    MovInst.addOperand(MCSrc);
    EmitToStreamer(*OutStreamer, MovInst);
    return;
  }
  case MCS51::MOVAnyDirect: {
    Register Rd = MI->getOperand(0).getReg();
    MCInst MovInst;
    MCOperand MCSrc;
    if (Rd == MCS51::ACC) {
      // translate load pseudo-move into MOV A, <direct>
      MovInst.setOpcode(MCS51::MOVADir);
    } else {
      // translate load pseudo-move into MOV Rn, <direct>
      MCOperand MCDst;
      MovInst.setOpcode(MCS51::MOVRnDir);
      lowerOperand(MI->getOperand(0), MCDst);
      MovInst.addOperand(MCDst);
    }
    lowerOperand(MI->getOperand(1), MCSrc);
    MovInst.addOperand(MCSrc);
    EmitToStreamer(*OutStreamer, MovInst);
    return;
  }
  case MCS51::MOVDirectAny: {
    Register Rs = MI->getOperand(1).getReg();
    MCInst MovInst;
    MCOperand MCDst;
    lowerOperand(MI->getOperand(0), MCDst);
    MovInst.addOperand(MCDst);
    if (Rs == MCS51::ACC) {
      // translate store pseudo-move into MOV <direct>, A
      MovInst.setOpcode(MCS51::MOVDirA);
    } else {
      // translate store pseudo-move into MOV <direct>, Rn
      MCOperand MCSrc;
      MovInst.setOpcode(MCS51::MOVDirRn);
      lowerOperand(MI->getOperand(1), MCSrc);
      MovInst.addOperand(MCSrc);
    }
    EmitToStreamer(*OutStreamer, MovInst);
    return;
  }
  }

  MCInst TmpInst;
  LowerMCS51MachineInstrToMCInst(MI, TmpInst, *this);
  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" void LLVMInitializeMCS51AsmPrinter() {
  RegisterAsmPrinter<MCS51AsmPrinter> X(getTheMCS51Target());
}

