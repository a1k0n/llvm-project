//===-- MCS51MCCodeEmitter.cpp - Convert MCS51 code to machine code -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MCS51MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {
class MCS51MCCodeEmitter : public MCCodeEmitter {
  MCS51MCCodeEmitter(const MCS51MCCodeEmitter &) = delete;
  void operator=(const MCS51MCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  MCContext &Ctx;

public:
 MCS51MCCodeEmitter(const MCInstrInfo &MCII, MCContext &ctx)
     : MCII(MCII), Ctx(ctx) {}

 ~MCS51MCCodeEmitter() override {}

 void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                        SmallVectorImpl<MCFixup> &Fixups,
                        const MCSubtargetInfo &   STI) const override;

 /// TableGen'erated function for getting the binary encoding for an
 /// instruction.
 uint64_t getBinaryCodeForInstr(const MCInst &            MI,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &   STI) const;

 /// Return binary encoding of operand. If the machine operand requires
 /// relocation, record the relocation and return zero.
 unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &   STI) const;
};
} // end anonymous namespace

MCCodeEmitter *llvm::createMCS51MCCodeEmitter(const MCInstrInfo &MCII,
                                              const MCRegisterInfo &MRI,
                                              MCContext &Ctx) {
  return new MCS51MCCodeEmitter(MCII, Ctx);
}

void MCS51MCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  unsigned Size = Desc.getSize();

  assert(Size > 0 && "Instruction size cannot be zero");

  uint64_t BinaryOpCode = getBinaryCodeForInstr(MI, Fixups, STI);
  while (Size > 0) {
    OS.write(BinaryOpCode & 0xff);
    BinaryOpCode >>= 8;
  }
  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

unsigned
MCS51MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {

  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());

  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  llvm_unreachable("Unhandled expression!");
  return 0;
}

#include "MCS51GenMCCodeEmitter.inc"
