//===-- MCS51ELFObjectWriter.cpp - MCS51 ELF Writer -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class MCS51ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit MCS51ELFObjectWriter(uint8_t OSABI);

  ~MCS51ELFObjectWriter() override;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

MCS51ELFObjectWriter::MCS51ELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit=*/false, OSABI, ELF::EM_8051,
                              /*HasRelocationAddend*/ true) {}

MCS51ELFObjectWriter::~MCS51ELFObjectWriter() {}

unsigned MCS51ELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  
switch ((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_Data_1:
    return ELF::R_MCS51_ADDR8;
  case FK_Data_2:
    return ELF::R_MCS51_ADDR16;
  }
}

std::unique_ptr<MCObjectTargetWriter> llvm::createMCS51ELFObjectWriter(
    uint8_t OSABI) {
  return std::make_unique<MCS51ELFObjectWriter>(OSABI);
}
