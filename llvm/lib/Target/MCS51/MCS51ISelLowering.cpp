//===-- MCS51ISelLowering.cpp - MCS51 DAG Lowering Implementation  --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that MCS51 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#include "MCS51ISelLowering.h"
#include "MCS51.h"
#include "MCS51RegisterInfo.h"
#include "MCS51Subtarget.h"
#include "MCS51TargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "MCS51-lower"

MCS51TargetLowering::MCS51TargetLowering(const TargetMachine &TM,
                                         const MCS51Subtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  // Set up the register classes.
  addRegisterClass(MVT::i8, &MCS51::GPRnRegClass);
  addRegisterClass(MVT::i8, &MCS51::GPRiRegClass);
  addRegisterClass(MVT::i8, &MCS51::ACCClassRegClass);
  addRegisterClass(MVT::i8, &MCS51::SPClassRegClass);
  addRegisterClass(MVT::i8, &MCS51::GPRAnyRegClass);

  addRegisterClass(MVT::i16, &MCS51::DPTRClassRegClass);

  // Compute derived properties from the register classes.
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(MCS51::SP);

  // TODO: add all necessary setOperationAction calls.
  setOperationAction(ISD::BR_CC, MVT::i8, Custom);
  setOperationAction(ISD::BRCOND, MVT::i8, Expand);
  setOperationAction(ISD::SELECT, MVT::i8, Expand);

  // TODO: take care of carrying addition / borrowing subtraction
  // setOperationAction(ISD::ADDC, MVT::i8, Custom);
  // setOperationAction(ISD::SUB, MVT::i8, Custom);

  // not totally sure about this; it almost doesn't matter if CF is used for
  // bool returns
  setBooleanContents(ZeroOrOneBooleanContent);

  // Function alignments (log2).
  setMinFunctionAlignment(Align(1));
  setPrefFunctionAlignment(Align(1));
}

SDValue MCS51TargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  case ISD::BR_CC: {
    SDValue Chain = Op.getOperand(0);
    ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
    SDValue LHS = Op.getOperand(2);
    SDValue RHS = Op.getOperand(3);
    SDValue Dest = Op.getOperand(4);
    SDLoc Dl(Op);
    switch (CC) {
    default:
      report_fatal_error("unimplemented condition code");
    case ISD::SETEQ:
    case ISD::SETNE:
      if (LHS.getOpcode() == ISD::Constant) {
        std::swap(LHS, RHS);
      }
      // If we're doing SETEQ/SETNE with zero, then just generate a JZ/JNZ
      SDValue Diff = LHS;
      if (RHS.getOpcode() == ISD::Constant) {
        if (cast<ConstantSDNode>(RHS)->getZExtValue() == 0) {
          // If the comparison happens to be (x-1) == 0, then we can use DJNZ
          if (CC == ISD::SETNE && LHS.getOpcode() == ISD::ADD &&
              LHS.getOperand(1).getOpcode() == ISD::Constant &&
              cast<ConstantSDNode>(LHS.getOperand(1))->getSExtValue() == -1) {
            if (LHS.getNode()->use_size() == 2) {
              // If there are 2 uses, assume it's a register copy and the BR_CC
              // itself now we need to get rid of the CopyToReg use so we'll
              // just replace it with the un-decremented input value and DJNZ
              // will actually decrement it.

              // This is probably not the right way to do this.
              DAG.ReplaceAllUsesOfValueWith(LHS, LHS.getOperand(0));
              return DAG.getNode(MCS51ISD::DJNZ, Dl, Op.getValueType(), Chain,
                                 LHS.getOperand(0), Dest);
            }
          }
        } else if (CC == ISD::SETNE) {
          // comparing with nonzero constant; candidate for CJNE
          return DAG.getNode(MCS51ISD::CJNE, Dl, Op.getValueType(), Chain, LHS,
                             RHS, Dest);
        } else {
          // TODO: reverse the branch!

          // Otherwise we need to XOR LHS with RHS to check for equality
          // (XOR is chosen insetad of subtract because subtract is always done
          // with carry on 8051, so we can avoid clearing carry this way)
          Diff = DAG.getNode(ISD::XOR, Dl, MVT::i8, LHS, RHS);
        }
      } else {
        // non-constant RHS; still need to generate a comparison
        Diff = DAG.getNode(ISD::XOR, Dl, MVT::i8, LHS, RHS);
      }
      Chain = DAG.getCopyToReg(Chain, Dl, MCS51::ACC, Diff);
      unsigned Opcode = CC == ISD::SETEQ ? MCS51ISD::JZ : MCS51ISD::JNZ;
      return DAG.getNode(Opcode, Dl, Op.getValueType(), Chain, Dest);
    }
  } break;
  }
  return Op;
}

// Calling Convention Implementation.
#include "MCS51GenCallingConv.inc"

// Transform physical registers into virtual registers.
SDValue MCS51TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::C:
    break;
  }

  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  if (IsVarArg)
    report_fatal_error("VarArg not supported");

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_MCS51);

  for (auto &VA : ArgLocs) {
    if (!VA.isRegLoc())
      report_fatal_error("Defined with too many args");

    // Arguments passed in registers.
    EVT RegVT = VA.getLocVT();
    if (RegVT != MVT::i8) {
      LLVM_DEBUG(dbgs() << "LowerFormalArguments Unhandled argument type: "
                        << RegVT.getEVTString() << "\n");
      report_fatal_error("unhandled argument type");
    }
    const unsigned VReg =
      RegInfo.createVirtualRegister(&MCS51::GPMovableRegClass);
    RegInfo.addLiveIn(VA.getLocReg(), VReg);
    SDValue ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);

    InVals.push_back(ArgIn);
  }
  return Chain;
}

SDValue
MCS51TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  if (IsVarArg) {
    report_fatal_error("VarArg not supported");
  }

  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_MCS51);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  // FIXME: ok, what's ISD?
  return DAG.getNode(MCS51ISD::RET_FLAG, DL, MVT::Other, RetOps);
}

const char *MCS51TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((MCS51ISD::NodeType)Opcode) {
  case MCS51ISD::FIRST_NUMBER:
    break;
  case MCS51ISD::RET_FLAG:
    return "MCS51ISD::RET_FLAG";
  case MCS51ISD::RETI_FLAG:
    return "MCS51ISD::RETI_FLAG";
  case MCS51ISD::JZ:
    return "MCS51ISD::JZ";
  case MCS51ISD::JNZ:
    return "MCS51ISD::JNZ";
  case MCS51ISD::JC:
    return "MCS51ISD::JC";
  case MCS51ISD::JNC:
    return "MCS51ISD::JNC";
  case MCS51ISD::DJNZ:
    return "MCS51ISD::DJNZ";
  case MCS51ISD::CJNE:
    return "MCS51ISD::CJNE";
  case MCS51ISD::ADD:
    return "MCS51ISD::ADD";
  case MCS51ISD::ADDC:
    return "MCS51ISD::ADDC";
  case MCS51ISD::SUBB:
    return "MCS51ISD::SUBB";
  }
  return nullptr;
}

