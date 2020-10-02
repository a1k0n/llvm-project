include "MCS51InstrFormats.td"

//===----------------------------------------------------------------------===//
// MCS51 specific DAG Nodes.
//===----------------------------------------------------------------------===//

def RetFlag  : SDNode<"MCS51ISD::RET_FLAG", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def RetIFlag : SDNode<"MCS51ISD::RETI_FLAG", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDTDJNZ : SDTypeProfile<0, 2, [
  SDTCisInt<0>, SDTCisVT<1, OtherVT>,
]>;

def SDTCJNE : SDTypeProfile<0, 3, [
  SDTCisSameAs<0, 1>, SDTCisVT<2, OtherVT>,
]>;

def SDTCJNE0 : SDTypeProfile<0, 3, [
  SDTCisSameAs<0, 1>, SDTCisInt<2>,
]>;

def SDTDADDC : SDTypeProfile<2, 3, [
  SDTCisSameAs<0, 1>, SDTCisInt<2>,
]>;

def MCS51JNZ  : SDNode<"MCS51ISD::JNZ", SDTBr, [SDNPHasChain]>;
def MCS51JZ   : SDNode<"MCS51ISD::JZ",  SDTBr, [SDNPHasChain]>;
def MCS51JNC  : SDNode<"MCS51ISD::JNC", SDTBr, [SDNPHasChain, SDNPOptInGlue]>;
def MCS51JC   : SDNode<"MCS51ISD::JC",  SDTBr, [SDNPHasChain, SDNPOptInGlue]>;
def MCS51CJNE : SDNode<"MCS51ISD::CJNE", SDTCJNE, [SDNPHasChain]>;
def MCS51CJNE0 : SDNode<"MCS51ISD::CJNE0", SDTCJNE0, [SDNPHasChain]>;
def MCS51DJNZ : SDNode<"MCS51ISD::DJNZ", SDTDJNZ, [SDNPHasChain]>;
def MCS51ADDC : SDNode<"MCS51ISD::ADDC", SDTDADDC, []>;
def MCS51SUBB : SDNode<"MCS51ISD::SUBB", SDTDADDC, []>;


class ImmAddrOperand<int width> : AsmOperandClass {
  let Name = "ImmAddr" # width;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

def addr16 : Operand<OtherVT> {
  let PrintMethod = "printAbsAddr16";
  let EncoderMethod = "encodeAbsBrTarget";
}

def bitaddr8 : Operand<i8>, ImmLeaf<i8, [{return isInt<8>(Imm);}]> {
  // addrspace?
  // decode SFR bits?
  // also hex?
}

def imm8 : Operand<i8>, ImmLeaf<i8, [{return isInt<8>(Imm);}]> {
  // also hex?
}

def reladdr8 : Operand<OtherVT> {
  let PrintMethod = "printPCRelImm8";
  let EncoderMethod = "encodeRelBrTarget";
}

def direct : Operand<i8>, ImmLeaf<i8, [{return isInt<8>(Imm);}]> {
  let PrintMethod = "printDirectA8";
  // TODO: custom parser / printer for register locations in RAM, and if
  // operand is >= 0x80, SFRs
}

def NOP : InstNoArgs<0, (outs), (ins), "NOP", "", []>;

let isTerminator = 1, isReturn = 1, isBarrier = 1 in {
  def RET  : InstNoArgs<0x22, (outs), (ins), "RET",  "", [(RetFlag)]>;
  def RETI : InstNoArgs<0x32, (outs), (ins), "RETI", "", [(RetIFlag)]>;
}

def INCDir : InsDir8<0, (outs), (ins direct:$a8),
 "INC", "$a8", [(store (add (i8 (load direct:$a8)), 1), direct:$a8)]>;
def DECDir : InsDir8<1, (outs), (ins direct:$a8),
 "DEC", "$a8", [(store (add (i8 (load direct:$a8)), -1), direct:$a8)]>;

let Constraints = "$dst = $rn" in {
  def INCRn : InsARn<0, (outs GPRn:$dst), (ins GPRn:$rn),
    "INC", "$rn", [(set GPRn:$dst, (add GPRn:$rn, 1))]>;
  def DECRn : InsARn<1, (outs GPRn:$dst), (ins GPRn:$rn),
    "DEC", "$rn", [(set GPRn:$dst, (add GPRn:$rn, -1))]>;
}

// not sure if the load/store patterns here are correct, as they lack
// address spaces, but these are specifically address space 0 (indirect
// access to internal RAM, not SFRs) which is the default
def INCRi : InsRi<0, (outs), (ins GPRi:$ri),
  "INC", "@$ri", [(store (add (i8 (load GPRi:$ri)), 1), GPRi:$ri)]>;
def DECRi : InsRi<1, (outs), (ins GPRi:$ri),
  "DEC", "@$ri", [(store (add (i8 (load GPRi:$ri)), -1), GPRi:$ri)]>;

// INC A is intentionally put last here so that greedy selection will pick INC
// Rn first to avoid register copies if possible.
let Uses = [ACC], Defs = [ACC] in {
  def INCA : InstNoArgs<0x04, (outs), (ins),
    "INC", "A", [(set ACC, (add ACC, 1))]>;
  def DECA : InstNoArgs<0x14, (outs), (ins),
    "DEC", "A", [(set ACC, (add ACC, -1))]>;
}

let Defs = [ACC], isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def CLRA : InstNoArgs<0xE4, (outs), (ins), "CLR", "A", [(set ACC, 0)]>;
  let Uses = [ACC] in {
    def CPLA : InstNoArgs<0xF4, (outs), (ins), "CPL", "A", [(set ACC, (xor ACC, -1))]>;
  }
}

let Defs = [CF], isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def CLRC : InstNoArgs<0xC3, (outs), (ins), "CLR", "C", [(set CF, 0)]>;
}

multiclass alu_inst<bits<4> opc, string mnemonic, dag op> {
  def _ARn : InsARn<opc, (outs), (ins GPRn:$rn), mnemonic, "A, $rn", [(set ACC, (op ACC, GPRn:$rn))]>;
  def _ARi : InsRi<opc, (outs), (ins GPRi:$ri), mnemonic, "A, @$ri", [(set ACC, (op ACC, GPRi:$ri))]>;
  def _Aimm8 : InsAImm8<opc, (outs), (ins imm8:$imm8), mnemonic, "A, #$imm8", [(set ACC, (op ACC, imm8:$imm8))]>;
  def _Adir8 : InsDir8<opc, (outs), (ins direct:$a8), mnemonic, "A, $a8", [(set ACC, (op ACC, (load direct:$a8)))]>;
}

multiclass alu_inst_carry<bits<4> opc, string mnemonic, dag op> {
  def _ARn : InsARn<opc, (outs), (ins GPRn:$rn), mnemonic, "A, $rn", [(set ACC, CF, (op ACC, GPRn:$rn, CF))]>;
  def _ARi : InsRi<opc, (outs), (ins GPRi:$ri), mnemonic, "A, @$ri", [(set ACC, CF, (op ACC, GPRi:$ri, CF))]>;
  def _Aimm8 : InsAImm8<opc, (outs), (ins imm8:$imm8), mnemonic, "A, #$imm8", [(set ACC, CF, (op ACC, imm8:$imm8, CF))]>;
  def _Adir8 : InsDir8<opc, (outs), (ins direct:$a8), mnemonic, "A, $a8", [(set ACC, CF, (op ACC, (load direct:$a8), CF))]>;
}

let Uses = [ACC], Defs = [ACC] in {
  defm ORL : alu_inst<4, "ORL", or>;
  defm ANL : alu_inst<5, "ANL", and>;
  defm XRL : alu_inst<6, "XRL", xor>;
}

let Uses = [ACC], Defs = [ACC, CF] in {
  defm ADD : alu_inst<2, "ADD", add>;
  let Uses = [ACC, CF] in {
    defm ADDC : alu_inst_carry<3, "ADDC", MCS51ADDC>;
    defm SUBB : alu_inst_carry<9, "SUBB", MCS51SUBB>;
  }
}

// FIXME: pattern for swapping A, Rn?
let Constraints = "$tmp = $rn" in {
  def XCHARn  : InsARn<0xC, (outs GPRn:$tmp), (ins GPRn:$rn),
    "XCH", "A, $rn", []>;
}

// Define all the various moves

// First check if we can avoid using a register at all
let mayLoad = 1, mayStore = 1 in {
  // note: on MOV iram8, iram8: opB is the destination (backwards from others)
  def MOVDirDir : InstOp8Op8<0x85, (outs), (ins direct:$opA, direct:$opB),
    "MOV", "$opB, $opA", [(store (i8 (load direct:$opA)), direct:$opB)]>;
}

// Start with pseudo-moves to cover the expanded set of MOV [A, Rn], loads and
// stores together; the actual instruction encoding will be chosen in pseudo
// expansion. This gives the register allocator more room to deal with loads and
// stores.
let isPseudo = 1, isCodeGenOnly = 1, isAsCheapAsAMove = 1 in {
  def MOVAnyImm : MCS51Inst<(outs GPMovable:$dst), (ins imm8:$imm8),
    "MOV", "$dst, #$imm8", [(set GPMovable:$dst, imm8:$imm8)]>;
  let mayLoad = 1 in {
  def MOVAnyDirect : MCS51Inst<(outs GPMovable:$dst), (ins direct:$src),
    "MOV", "$dst, $src", [(set GPMovable:$dst, (load direct:$src))]>;
  }
  let mayStore = 1 in {
  def MOVDirectAny : MCS51Inst<(outs), (ins direct:$dst, GPMovable:$src),
    "MOV", "$dst, $src", [(store GPMovable:$src, direct:$dst)]>;
  }
}

def MOVRnRn   : InstMovRnRdir<(outs GPRn:$rn), (ins GPRn:$rdir),
  "MOV", "$rn, $rdir", [(set GPRn:$rn, GPRn:$rdir)]>;
def MOVRnDir  : InstRnImm8<0xA, (outs GPRn:$rn), (ins direct:$imm8),
  "MOV", "$rn, $imm8", [(set GPRn:$rn, (load direct:$imm8))]>;
def MOVDirRn  : InstRnImm8<0x8, (outs), (ins direct:$imm8, GPRn:$rn),
  "MOV", "$imm8, $rn", [(store GPRn:$rn, direct:$imm8)]>;
let isMoveImm = 1 in {
  def MOVRnImm8 : InstRnImm8<0x7, (outs GPRn:$rn), (ins imm8:$imm8),
    "MOV", "$rn, #$imm8", [(set GPRn:$rn, imm8:$imm8)]>;
let mayStore = 1 in {
  def MOVDirImm : InstOp8Op8<0x75, (outs), (ins direct:$opA, imm8:$opB),
    "MOV", "$opA, #$opB", [(store imm8:$opB, direct:$opA)]>;
}
}

let Defs = [ACC] in {
  def MOVADir  : InsDir8<0xE, (outs), (ins direct:$a8),
    "MOV", "A, $a8", [(set ACC, (load direct:$a8))]>;
  def MOVAImm8 : InsAImm8<0x7, (outs), (ins imm8:$imm8),
    "MOV", "A, #$imm8", [(set ACC, imm8:$imm8)]>;
  def MOVARn   : InsARn<0xE, (outs), (ins GPRn:$rn),
    "MOV", "A, $rn", [(set ACC, GPRn:$rn)]>;
  def MOVARi   : InsRi<0xE, (outs), (ins GPRi:$ri),
    "MOV", "A, @$ri", [(set ACC, (load GPRi:$ri))]>;
}

let Uses = [ACC] in {
  def MOVDirA : InsDir8<0xF, (outs), (ins direct:$a8),
    "MOV", "$a8, A", [(store ACC, direct:$a8)]>;
  def MOVRnA  : InsARn<0xF, (outs GPRn:$rn), (ins),
    "MOV", "$rn, A", [(set GPRn:$rn, ACC)]>;
  def MOVRiA  : InsRi<0xF, (outs), (ins GPRi:$ri),
    "MOV", "@$ri, A", [(store ACC, GPRi:$ri)]>;
}

let Defs = [CF] in {
  def MOVCbit : InsImm8<0xA2, (outs), (ins bitaddr8:$imm8),
    "MOV", "C, bit $imm8", [(set CF, (load bitaddr8:$imm8))]>;
}
let Uses = [CF] in {
  def MOVbitC : InsImm8<0x92, (outs), (ins bitaddr8:$imm8),
    "MOV", "bit $imm8, C", [(store CF, bitaddr8:$imm8)]>;
}

// TODO: ACALL / AJMP are somewhat unlikely to be used by the compiler but
// should be dis/assemblable at least

def LCALL : InstAddr16<0x12, (outs), (ins addr16:$addr16), "LCALL", "$addr16", []>;

let isBranch = 1, isTerminator = 1 in {
let isBarrier = 1 in {
def LJMP : InstAddr16<0x02, (outs), (ins addr16:$addr16), "LJMP", "$addr16", []>;
def SJMP : InstRelAddr8<0x80, (outs), (ins reladdr8:$rel8),
  "SJMP", "$rel8", [(br bb:$rel8)]>;
}

// pattern is matched in MCS51ISelDAGToDAG; hence no pattern here
def DJNZ : InstRnRel8<0xD, (outs), (ins GPRn:$rn, reladdr8:$rel8),
  "DJNZ", "$rn, $rel8", [(MCS51DJNZ GPRn:$rn, bb:$rel8)]>;
def CJNERnimm : InstRnOp8Op8<0xB, (outs), (ins GPRn:$rn, imm8:$opA, reladdr8:$opB),
  "CJNE", "$rn, #$opA, $opB", [(MCS51CJNE GPRn:$rn, imm8:$opA, bb:$opB)]>;
let Uses = [ACC] in {
def CJNEAimm : InstOp8Op8<0xB4, (outs), (ins imm8:$opA, reladdr8:$opB),
  "CJNE", "A, #$opA, $opB", [(MCS51CJNE ACC, imm8:$opA, bb:$opB)]>;
  // TODO: CJNE A, Dir8, rel8
  //       CJNE @Ri, #imm8, rel8
}

let isPseudo = 1, isCodeGenOnly = 1 in {
def CJNEAnyimm0 : InstOp8Op8<0xB4, (outs), (ins GPMovable:$ra, imm8:$opA, imm8:$opB),
  "CJNE", "$ra, #$opA, $opB", [(MCS51CJNE0 GPMovable:$ra, imm8:$opA, imm8:$opB)]>;
}  // isPseudo, isCodeGenOnly

// Conditional branches
def JC  : InstRelAddr8<0x40, (outs), (ins reladdr8:$rel8),
  "JC", "$rel8", [(MCS51JC bb:$rel8)]>;
def JNC : InstRelAddr8<0x50, (outs), (ins reladdr8:$rel8),
  "JNC", "$rel8", [(MCS51JNC bb:$rel8)]>;
let Uses = [ACC] in {
def JZ  : InstRelAddr8<0x60, (outs), (ins reladdr8:$rel8),
  "JZ", "$rel8", [(MCS51JZ bb:$rel8)]>;
def JNZ : InstRelAddr8<0x70, (outs), (ins reladdr8:$rel8),
  "JNZ", "$rel8", [(MCS51JNZ bb:$rel8)]>;
}
} // isBranch, isTerminator