//===-- MCS51FrameLowering.h - Define frame lowering for MCS51 -*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements MCS51-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51FRAMELOWERING_H
#define LLVM_LIB_TARGET_MCS51_MCS51FRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class MCS51Subtarget;

class MCS51FrameLowering : public TargetFrameLowering {
public:
  explicit MCS51FrameLowering(const MCS51Subtarget &STI)
      : TargetFrameLowering(StackGrowsUp,
                            /*StackAlignment=*/Align(1),
                            /*LocalAreaOffset=*/0) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool hasFP(const MachineFunction &MF) const override;
};
}
#endif

