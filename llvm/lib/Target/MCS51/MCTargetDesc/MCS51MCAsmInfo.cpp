//===-- MCS51MCAsmInfo.cpp - MCS51 Asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the MCS51MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "MCS51MCAsmInfo.h"
#include "llvm/ADT/Triple.h"
using namespace llvm;

void MCS51MCAsmInfo::anchor() {}

MCS51MCAsmInfo::MCS51MCAsmInfo(const Triple &TT, const MCTargetOptions &Options) {
  CodePointerSize = CalleeSaveStackSlotSize = 2;
  CommentString = "#";
  // AlignmentIsInBytes = true; ??
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
  UsesELFSectionDirectiveForBSS = true;
  SupportsDebugInformation = true;
}
