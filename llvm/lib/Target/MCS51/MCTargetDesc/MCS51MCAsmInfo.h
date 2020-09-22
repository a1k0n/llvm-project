//===-- MCS51MCAsmInfo.h - MCS51 Asm Info ----------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MCS51MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCASMINFO_H
#define LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class MCS51MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit MCS51MCAsmInfo(const Triple &TT, const MCTargetOptions &Options);
};

} // namespace llvm

#endif

