//===-- MCS51Disassembler.cpp - Disassembler for MCS51 --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MCS51Disassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "MCS51-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {
class MCS51Disassembler : public MCDisassembler {

public:
  MCS51Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  virtual ~MCS51Disassembler() {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createMCS51Disassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new MCS51Disassembler(STI, Ctx);
}

extern "C" void LLVMInitializeMCS51Disassembler() {
  // Register the disassembler for each target.
  TargetRegistry::RegisterMCDisassembler(getTheMCS51Target(),
                                         createMCS51Disassembler);
}

static const unsigned GPRnDecoderTable[] = {
  MCS51::R0,  MCS51::R1,  MCS51::R2,  MCS51::R3,
  MCS51::R4,  MCS51::R5,  MCS51::R6,  MCS51::R7,
};

static DecodeStatus DecodeGPRnRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t    Address,
                                            const void *Decoder) {
  if (RegNo > sizeof(GPRnDecoderTable)) return MCDisassembler::Fail;

  // We must define our own mapping from RegNo to register identifier.
  // Accessing index RegNo in the register class will work in the case that
  // registers were added in ascending order, but not in general.
  unsigned Reg = GPRnDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeGPRiRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t    Address,
                                            const void *Decoder) {
  if (RegNo > 1) return MCDisassembler::Fail;
  return DecodeGPRnRegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus decodeImmAddr16Operand(MCInst &Inst, uint64_t Imm,
                                           int64_t Address,
                                           const void *Decoder) {
  assert(isUInt<16>(Imm) && "Invalid immediate address");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

/*
template <unsigned N>
static DecodeStatus decodeUImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<N>(Imm) && "Invalid immediate");
  // Sign-extend the number in the bottom N bits of Imm
  Inst.addOperand(MCOperand::createImm(SignExtend64<N>(Imm)));
  return MCDisassembler::Success;
}
*/

#include "MCS51GenDisassemblerTables.inc"

DecodeStatus MCS51Disassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t          Address,
                                               raw_ostream &CStream) const {
  Size = 1;
  if (Bytes.size() < 1) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  uint32_t Insn = Bytes[0];
  DecodeStatus r = decodeInstruction(DecoderTable8, Instr, Insn, Address, this, STI);
  if (r != MCDisassembler::Fail) {
    return r;
  }

  if (Bytes.size() < 2) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn |= (Bytes[1] << 8);
  Size = 2;
  r = decodeInstruction(DecoderTable16, Instr, Insn, Address, this, STI);
  if (r != MCDisassembler::Fail) {
    return r;
  }

  if (Bytes.size() < 3) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Insn |= (Bytes[2] << 16);
  Size = 3;
  return decodeInstruction(DecoderTable24, Instr, Insn, Address, this, STI);
}
