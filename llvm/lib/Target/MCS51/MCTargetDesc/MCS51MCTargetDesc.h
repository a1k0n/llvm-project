//===-- MCS51MCTargetDesc.h - MCS51 Target Descriptions ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MCS51 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCTARGETDESC_H
#define LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;

MCCodeEmitter *createMCS51MCCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo &MRI,
                                        MCContext &Ctx);

MCAsmBackend *createMCS51AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createMCS51ELFObjectWriter(uint8_t OSABI);
}  // namespace llvm

// Defines symbolic names for 8051 registers.
#define GET_REGINFO_ENUM
#include "MCS51GenRegisterInfo.inc"

// Defines symbolic names for 8051 instructions.
#define GET_INSTRINFO_ENUM
#include "MCS51GenInstrInfo.inc"

#endif

