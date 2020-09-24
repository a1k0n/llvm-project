include "MCS51InstrFormats.td"

class ImmAddrOperand<int width> : AsmOperandClass {
  let Name = "ImmAddr" # width;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

def addr16 : Operand<i16> {
}

def imm8 : Operand<i8> {
  // also hex?
}

def direct : Operand<i8> {
  // TODO: custom parser / printer for register locations in RAM, and if
  // operand is >= 0x80, SFRs
}

def NOP : InstNoArgs<0, (outs), (ins), "NOP", "", []>;

def RET  : InstNoArgs<0x22, (outs), (ins), "RET",  "", []>;
def RETI : InstNoArgs<0x32, (outs), (ins), "RETI", "", []>;

let Uses = [ACC], Defs = [ACC] in {
  def INCA : InstNoArgs<0x04, (outs), (ins),
    "INC", "A", [(set ACC, (add ACC, 1))]>;
}

let Defs = [ACC] in {
  def CLRA : InstNoArgs<0xE4, (outs), (ins), "CLR", "A", [(set ACC, 0)]>;
}

let Constraints = "$dst = $rn" in {
  def INCRn : InsARn<0, (outs GPRn:$dst), (ins GPRn:$rn),
    "INC", "$rn", [(set GPRn:$dst, (add GPRn:$rn, 1))]>;
  def DECRn : InsARn<1, (outs GPRn:$dst), (ins GPRn:$rn),
    "DEC", "$rn", [(set GPRn:$dst, (sub GPRn:$rn, 1))]>;
}

let Constraints = "$dst = $ri" in {
  // not sure if the load/store patterns here are correct, as they lack
  // address spaces, but these are specifically address space 0 (indirect
  // access to internal RAM, not SFRs) which is the default
  def INCRi : InsARi<0, (outs GPRi:$dst), (ins GPRi:$ri),
    "INC", "@$ri", [(store GPRi:$dst, (add (load GPRi:$ri), 1))]>;
  def DECRi : InsARi<1, (outs GPRi:$dst), (ins GPRi:$ri),
    "DEC", "@$ri", [(store GPRi:$dst, (sub (load GPRi:$ri), 1))]>;
}

def INCDir : InsDir8<0, (outs), (ins direct:$a8),
 "INC", "$a8", [(store direct:$a8, (add (load direct:$a8), 1))]>;
def DECDir : InsDir8<1, (outs), (ins direct:$a8),
 "DEC", "$a8", [(store direct:$a8, (sub (load direct:$a8), 1))]>;

multiclass alu_inst<bits<4> opc, string mnemonic, dag op> {
  def _ARn : InsARn<opc, (outs), (ins GPRn:$rn), mnemonic, "A, $rn", [(set ACC, (op ACC, GPRn:$rn))]>;
  def _ARi : InsARi<opc, (outs), (ins GPRi:$ri), mnemonic, "A, @$ri", [(set ACC, (op ACC, GPRi:$ri))]>;
  def _Aimm8 : InsImm8<opc, (outs), (ins imm8:$imm8), mnemonic, "A, #$imm8", [(set ACC, (op ACC, imm8:$imm8))]>;
  def _Adir8 : InsDir8<opc, (outs), (ins direct:$a8), mnemonic, "A, $a8", [(set ACC, (op ACC, (load direct:$a8)))]>;
}

let Uses = [ACC], Defs = [ACC, PSW] in {
  defm ADD  : alu_inst<2, "ADD",  add>;
  defm ADDC : alu_inst<3, "ADDC", addc>;
  defm SUBB : alu_inst<9, "SUBB", subc>;
}

let Uses = [ACC], Defs = [ACC] in {
  defm ORL : alu_inst<4, "ORL", or>;
  defm ANL : alu_inst<5, "ANL", and>;
  defm XRL : alu_inst<6, "XRL", xor>;
}

// FIXME: pattern for swapping A, Rn?
let Constraints = "$tmp = $rn" in {
  def XCHARn  : InsARn<0xC, (outs GPRn:$tmp), (ins GPRn:$rn),
  "XCH", "A, $rn", []>;
}

def MOVAimm8 : InsImm8<0x7, (outs), (ins imm8:$imm8),
 "MOV", "A, #$imm8", [(set ACC, imm8:$imm8)]>;
def MOVARn  : InsARn<0xE, (outs), (ins GPRn:$rn),
 "MOV", "A, $rn", [(set ACC, GPRn:$rn)]>;
def MOVRnA  : InsARn<0xF, (outs GPRn:$rn), (ins),
 "MOV", "$rn, A", [(set GPRn:$rn, ACC)]>;


// TODO: ACALL / AJMP are somewhat unlikely to be used by the compiler but
// should be dis/assemblable at least

def LCALL : MCS51InstAddr16<0x12, (outs), (ins addr16:$addr16), "LCALL", "$addr16", []>;
def LJMP : MCS51InstAddr16<0x02, (outs), (ins addr16:$addr16), "LJMP", "$addr16", []>;
