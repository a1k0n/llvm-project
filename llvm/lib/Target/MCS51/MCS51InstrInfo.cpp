//===-- MCS51InstrInfo.cpp - MCS51 Instruction Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MCS51 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MCS51InstrInfo.h"
#include "MCS51.h"
#include "MCS51Subtarget.h"
#include "MCS51TargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "MCS51GenInstrInfo.inc"

using namespace llvm;

#define DEBUG_TYPE "mcs51-instrinfo"

MCS51InstrInfo::MCS51InstrInfo() : MCS51GenInstrInfo() {}

void MCS51InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MBBI,
                                 const DebugLoc &DL, MCRegister DstReg,
                                 MCRegister SrcReg, bool KillSrc) const {
  if (DstReg == MCS51::ACC) {
    if (MCS51::GPRnRegClass.contains(SrcReg)) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVARn))
          .addReg(SrcReg, getKillRegState(KillSrc));
      return;
    } else {
      // FIXME
      llvm_unreachable("trying to move non-Rn into A unimplemented");
    }
  } else if (SrcReg == MCS51::ACC) {
    if (MCS51::GPRnRegClass.contains(DstReg)) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVRnA), DstReg);
      return;
    } else {
      llvm_unreachable("trying to move non-Rn into A unimplemented");
    }
  } else if (MCS51::GPRnRegClass.contains(SrcReg, DstReg)) {
    BuildMI(MBB, MBBI, DL, get(MCS51::MOVRnRn), DstReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  } else if (SrcReg == MCS51::CF) {
    if (DstReg == MCS51::CFslack) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVbitC)).addImm(0x08);
      return;
    }
  } else if (DstReg == MCS51::CF) {
    if (SrcReg == MCS51::CFslack) {
      BuildMI(MBB, MBBI, DL, get(MCS51::MOVCbit)).addImm(0x08);
      return;
    }
  }
  LLVM_DEBUG(dbgs() << "copyPhysReg " << DstReg << " <- " << SrcReg << "\n");
  llvm_unreachable("unimplemented copyPhysReg x, y");
}

bool MCS51InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                   MachineBasicBlock *&TBB,
                                   MachineBasicBlock *&FBB,
                                   SmallVectorImpl<MachineOperand> &Cond,
                                   bool AllowModify) const {
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugInstr())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(*I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analysis.
    if (!I->isBranch())
      return true;

    // Cannot handle indirect branches.
    // TODO: indirect branches
    //if (I->getOpcode() == MSP430::Br ||
    //    I->getOpcode() == MSP430::Bm)
    //  return true;

    // Handle unconditional branches.
    if (I->getOpcode() == MCS51::SJMP) {
      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JMP, delete them.
      while (std::next(I) != MBB.end())
        std::next(I)->eraseFromParent();
      Cond.clear();
      FBB = nullptr;

      // Delete the JMP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = nullptr;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }

      // TBB is used to indicate the unconditinal destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    // Handle conditional branches.
    /*
    assert(I->getOpcode() == MSP430::JCC && "Invalid conditional branch");
    MSP430CC::CondCodes BranchCode =
      static_cast<MSP430CC::CondCodes>(I->getOperand(1).getImm());
    if (BranchCode == MSP430CC::COND_INVALID)
      return true;  // Can't handle weird stuff.

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty()) {
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCode));
      continue;
    }

    // Handle subsequent conditional branches. Only handle the case where all
    // conditional branches branch to the same destination.
    assert(Cond.size() == 1);
    assert(TBB);

    // Only handle the case where all conditional branches branch to
    // the same destination.
    if (TBB != I->getOperand(0).getMBB())
      return true;

    MSP430CC::CondCodes OldBranchCode = (MSP430CC::CondCodes)Cond[0].getImm();
    // If the conditions are the same, we can leave them alone.
    if (OldBranchCode == BranchCode)
      continue;
    */

    return true;
  }

  return false;
}

// Inserts a branch into the end of the specific MachineBasicBlock, returning
// the number of instructions inserted.
unsigned MCS51InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  assert(!BytesAdded && "Code size not handled.");

  // Unconditional branch.
  if (Cond.empty()) {
    BuildMI(&MBB, DL, get(MCS51::SJMP)).addMBB(TBB);
    return 1;
  }

  // Either a one or two-way conditional branch.
  LLVM_DEBUG(dbgs() << "attempting to insert branch type"; Cond[0].dump());
  return 0;

/*
  unsigned Opc = Cond[0].getImm();
  BuildMI(&MBB, DL, get(Opc)).add(Cond[1]).add(Cond[2]).addMBB(TBB);

  // One-way conditional branch.
  if (!FBB)
    return 1;

  // Two-way conditional branch.
  BuildMI(&MBB, DL, get(MCS51::SJMP)).addMBB(FBB);
  return 2;
  */
}

unsigned MCS51InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                      int *BytesRemoved) const {
  assert(!BytesRemoved && "Code size not handled");
  MachineBasicBlock::iterator I = MBB.getLastNonDebugInstr();
  if (I == MBB.end())
    return 0;
  LLVM_DEBUG(dbgs() << "analyzeBranch->removeBranch: "; I->dump());

  if (!I->getDesc().isUnconditionalBranch() &&
      !I->getDesc().isConditionalBranch())
    return 0;

  // Remove the branch.
  I->eraseFromParent();
  return 1;
  /*

  I = MBB.end();

  if (I == MBB.begin())
    return 1;
  --I;
  if (!I->getDesc().isConditionalBranch())
    return 1;

  // why would i remove the conditional branch?!?
  // Remove the branch.
  I->eraseFromParent();
  return 2;
  */
}

bool MCS51InstrInfo::reverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  LLVM_DEBUG(dbgs() << "reverseBranchCondition(siz=" << Cond.size()
                    << ", cond[0] = ";
             Cond[0].dump());
  return true;
}

bool MCS51InstrInfo::isReallyTriviallyReMaterializable(const MachineInstr &MI,
                                                     AAResults *AA) const {
  // this should only be called for CLRs
  LLVM_DEBUG(dbgs() << "isReallyTriviallyMaterializable: "; MI.dump());
  return true;
}