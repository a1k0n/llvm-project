//===-- MCS51InstPrinter.cpp - Convert MCS51 MCInst to asm syntax ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an MCS51 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MCS51InstPrinter.h"
#include "llvm/ADT/Twine.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#include "MCS51GenAsmWriter.inc"

MCS51InstPrinter::~MCS51InstPrinter() {}

void MCS51InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annot, const MCSubtargetInfo &STI,
                                 raw_ostream &OS) {
  printInstruction(MI, Address, OS);
  printAnnotation(OS, Annot);
}

void MCS51InstPrinter::printRegName(raw_ostream &O, unsigned RegNo) const {
  O << getRegisterName(RegNo);
}

void MCS51InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &O, const char *Modifier) {
  assert((Modifier == 0 || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &MO = MI->getOperand(OpNo);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return;
  }

  if (MO.isImm()) {
    O << MO.getImm();
    return;
  }

  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}

/// This is used to print an immediate value that ends up
/// being encoded as a pc-relative value.
void MCS51InstPrinter::printPCRelImm8(const MCInst *MI, unsigned OpNo,
                                      raw_ostream &O) {
  if (OpNo >= MI->size()) {
    // Not all operands are correctly disassembled at the moment. This means
    // that some machine instructions won't have all the necessary operands
    // set.
    // To avoid asserting, print <unknown> instead until the necessary support
    // has been implemented.
    O << "<unknown>";
    return;
  }

  const MCOperand &Op = MI->getOperand(OpNo);

  if (Op.isImm()) {
    // sign extend
    int64_t Imm = static_cast<int64_t>(static_cast<int8_t>(Op.getImm()));
    O << '.';

    // Print a position sign if needed.
    // Negative values have their sign printed automatically.
    if (Imm >= 0)
      O << '+';

    O << Imm;
  } else {
    assert(Op.isExpr() && "Unknown pcrel immediate operand");
    O << *Op.getExpr();
  }
}

void MCS51InstPrinter::printAbsAddr16(const MCInst *MI, unsigned OpNo,
                                      raw_ostream &O) {
  if (OpNo >= MI->size()) {
    // Not all operands are correctly disassembled at the moment. This means
    // that some machine instructions won't have all the necessary operands
    // set.
    // To avoid asserting, print <unknown> instead until the necessary support
    // has been implemented.
    O << "<unknown>";
    return;
  }

  const MCOperand &Op = MI->getOperand(OpNo);

  if (Op.isImm()) {
    uint16_t Imm = Op.getImm();

    O << "0x" << Twine::utohexstr(Imm) << "\n";
  } else {
    assert(Op.isExpr() && "Unknown pcrel immediate operand");
    O << *Op.getExpr();
  }
}