//===-- MCS51AsmBackend.cpp - MCS51 Assembler Backend ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class MCS51AsmBackend : public MCAsmBackend {
public:
  explicit MCS51AsmBackend(Triple::OSType OSType)
      : MCAsmBackend(support::little), OSType(OSType) {}
  ~MCS51AsmBackend() override {}

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                            MCContext &Ctx) const;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  unsigned getNumFixupKinds() const override { return 1; }

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override;

private:
  Triple::OSType OSType;
};

bool MCS51AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count) const {
  // conveniently enough, NOP is 00 on 8051.
  OS.write_zeros(Count);

  return true;
}

uint64_t MCS51AsmBackend::adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                           MCContext &Ctx) const {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  case FK_PCRel_1:
    // adjust value to point to next instruction (all PCRel_1 insns have the
    // same form)
    return Value - 1;
  default:
    return Value;
  }
}

void MCS51AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                 const MCValue &Target,
                                 MutableArrayRef<char> Data, uint64_t Value,
                                 bool IsResolved,
                                 const MCSubtargetInfo *STI) const {
  MCContext &Ctx = Asm.getContext();
  Value = adjustFixupValue(Fixup, Value, Ctx);

  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());
  Value <<= Info.TargetOffset;
  unsigned Offset = Fixup.getOffset();
  unsigned NumBytes = (Info.TargetSize + 7) / 8;
#ifndef NDEBUG
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");
#endif

  // For each byte of the fragment that the fixup touches, mask in the
  // bits from the fixup value.
  for (unsigned i = 0; i < NumBytes; ++i) {
    Data[Offset + i] |= Value & 0xff;
    Value >>= 8;
  }
}

std::unique_ptr<MCObjectTargetWriter>
MCS51AsmBackend::createObjectTargetWriter() const {
  return createMCS51ELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

} // end anonymous namespace

MCAsmBackend *llvm::createMCS51AsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  const Triple &TT = STI.getTargetTriple();
  return new MCS51AsmBackend(TT.getOS());
}
