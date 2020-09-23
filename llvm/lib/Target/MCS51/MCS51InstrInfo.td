include "MCS51InstrFormats.td"

def addr16 : Operand<i16> {
    // FIXME: address printing should be in hex or label-relative
}

def imm8 : Operand<i8> {
    // also hex?
}

def NOP : MCS51InstNoArgs<0, (outs), (ins), "NOP", "", []>;
// what pattern would we use for RET?
def RET : MCS51InstNoArgs<0x22, (outs), (ins), "RET", "", []>;
def RETI : MCS51InstNoArgs<0x32, (outs), (ins), "RETI", "", []>;

let Constraints = "$dst = $ra" in {
    def INCA : MCS51InstNoArgs<0x04, (outs AREG:$dst), (ins AREG:$ra),
     "INC", "$ra", [(set AREG:$dst, (add AREG:$ra, 1))]>;
}
def CLRA : MCS51InstNoArgs<0xE4, (outs AREG:$ra), (ins), "CLR", "$ra", [(set AREG:$ra, 0)]>;

let Constraints = "$dst = $rn" in {
    def INCRn : MCS51InstRn<0, (outs GPRn:$dst), (ins GPRn:$rn),
     "INC", "$rn", [(set GPRn:$dst, (add GPRn:$rn, 1))]>;
    def DECRn : MCS51InstRn<1, (outs GPRn:$dst), (ins GPRn:$rn),
     "DEC", "$rn", [(set GPRn:$dst, (sub GPRn:$rn, 1))]>;
}

let Constraints = "$dst = $ri" in {
    // not sure if the load/store patterns here are correct, as they lack
    // address spaces, but these are specifically address space 0 (indirect
    // access to internal RAM, not SFRs) which is the default
    def INCRi : MCS51InstRi<0, (outs GPRi:$dst), (ins GPRi:$ri),
     "INC", "@$ri", [(store GPRi:$dst, (add (load GPRi:$ri), 1))]>;
    def DECRi : MCS51InstRi<1, (outs GPRi:$dst), (ins GPRi:$ri),
     "DEC", "@$ri", [(store GPRi:$dst, (sub (load GPRi:$ri), 1))]>;
}

let Defs = [PSW], Constraints = "$dst = $ra" in {
    def ADDARn  : MCS51InstRn<2, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ADD", "$ra, $rn", [(set AREG:$dst, (add AREG:$ra, GPRn:$rn))]>;
    def ADDARi  : MCS51InstRi<2, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "ADD", "$ra, @$ri", [(store AREG:$dst, (add AREG:$ra, (load GPRi:$ri)))]>;
    def ADDAimm8 : MCS51InstImm8<2, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "ADD", "$ra, #$imm8", [(set AREG:$dst, (add AREG:$ra, imm8:$imm8))]>;
    // FIXME: carry / borrow -- are addc and subc correct here?
    def ADDCARn : MCS51InstRn<3, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ADDC", "$ra, $rn", [(set AREG:$dst, (addc AREG:$ra, GPRn:$rn))]>;
    def ADDCARi : MCS51InstRi<3, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "ADDC", "$ra, @$ri", [(store AREG:$dst, (addc AREG:$ra, (load GPRi:$ri)))]>;
    def ADDCAimm8 : MCS51InstImm8<3, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "ADDC", "$ra, #$imm8", [(set AREG:$dst, (addc AREG:$ra, imm8:$imm8))]>;
    def SUBBARn : MCS51InstRn<9, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "SUBB", "$ra, $rn", [(set AREG:$dst, (subc AREG:$ra, GPRn:$rn))]>;
    def SUBBARi : MCS51InstRi<9, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "SUBB", "$ra, @$ri", [(store AREG:$dst, (subc AREG:$ra, (load GPRi:$ri)))]>;
    def SUBBAimm8 : MCS51InstImm8<9, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "SUBB", "$ra, #$imm8", [(set AREG:$dst, (subc AREG:$ra, imm8:$imm8))]>;
}

let Constraints = "$dst = $ra" in {
    def ORLARn  : MCS51InstRn<4, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ORL", "$ra, $rn", [(set AREG:$dst, (or AREG:$ra, GPRn:$rn))]>;
    def ORLARi  : MCS51InstRi<4, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "ORL", "$ra, @$ri", [(set AREG:$dst, (or AREG:$ra, (load GPRi:$ri)))]>;
    def ORLAimm8 : MCS51InstImm8<4, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "ORL", "$ra, #$imm8", [(set AREG:$dst, (or AREG:$ra, imm8:$imm8))]>;
    def ANLARn  : MCS51InstRn<5, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ANL", "$ra, $rn", [(set AREG:$dst, (and AREG:$ra, GPRn:$rn))]>;
    def ANLARi  : MCS51InstRi<5, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "ANL", "$ra, @$ri", [(set AREG:$dst, (and AREG:$ra, (load GPRi:$ri)))]>;
    def ANLAimm8 : MCS51InstImm8<4, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "ANL", "$ra, #$imm8", [(set AREG:$dst, (and AREG:$ra, imm8:$imm8))]>;
    def XRLARn  : MCS51InstRn<6, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "XRL", "$ra, $rn", [(set AREG:$dst, (xor AREG:$ra, GPRn:$rn))]>;
    def XRLARi  : MCS51InstRi<6, (outs AREG:$dst), (ins AREG:$ra, GPRi:$ri),
     "XRL", "$ra, @$ri", [(set AREG:$dst, (xor AREG:$ra, (load GPRi:$ri)))]>;
    def XRLAimm8 : MCS51InstImm8<6, (outs AREG:$dst), (ins AREG:$ra, imm8:$imm8),
     "XRL", "$ra, #$imm8", [(set AREG:$dst, (xor AREG:$ra, imm8:$imm8))]>;
}

// FIXME: pattern for swapping A, Rn?
let Constraints = "$t1 = $ra, $t2 = $rn" in {
    def XCHARn  : MCS51InstRn<0xC, (outs AREG:$t1, GPRn:$t2), (ins AREG:$ra, GPRn:$rn),
    "XCH", "$ra, $rn", []>;
}

def MOVAimm8 : MCS51InstImm8<0x7, (outs AREG:$ra), (ins imm8:$imm8),
 "MOV", "$ra, #$imm8", [(set AREG:$ra, imm8:$imm8)]>;
def MOVARn  : MCS51InstRn<0xE, (outs AREG:$ra), (ins GPRn:$rn),
 "MOV", "$ra, $rn", [(set AREG:$ra, GPRn:$rn)]>;
def MOVRnA  : MCS51InstRn<0xF, (outs GPRn:$rn), (ins AREG:$ra),
 "MOV", "$rn, $ra", [(set GPRn:$rn, AREG:$ra)]>;

def LJMP : MCS51InstAddr16<0x02, (outs), (ins addr16:$addr16), "LJMP", "$addr16", []>;
