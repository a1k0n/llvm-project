//===-- MCS51.td - Describe the MCS51 Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// 8051 subtarget features and instruction predicates.
//===----------------------------------------------------------------------===//

// Theoretically we could begin to support MCS251 or MX51 with some flags here

//===----------------------------------------------------------------------===//
// Register file, instruction descriptions.
//===----------------------------------------------------------------------===//

include "MCS51RegisterInfo.td"
include "MCS51InstrInfo.td"

def MCS51InstrInfo : InstrInfo;

def MCS51AsmParser : AsmParser {
}

//===----------------------------------------------------------------------===//
// 8051 processors supported.
//===----------------------------------------------------------------------===//

def : ProcessorModel<"generic-8051", NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// Define the MCS51 target.
//===----------------------------------------------------------------------===//

def MCS51 : Target {
  let InstructionSet = MCS51InstrInfo;
  let AssemblyParsers = [MCS51AsmParser];
}

