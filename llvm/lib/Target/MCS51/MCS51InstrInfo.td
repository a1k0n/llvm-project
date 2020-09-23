include "MCS51InstrFormats.td"

def NOP : MCS51InstNoArgs<0, (outs), (ins), "NOP", "", []>;
// what pattern would we use for RET?
def RET : MCS51InstNoArgs<0x22, (outs), (ins), "RET", "", []>;
def RETI : MCS51InstNoArgs<0x32, (outs), (ins), "RETI", "", []>;

let Constraints = "$dst = $rn" in {
    def INCRn : MCS51InstRn<0, (outs GPRn:$dst), (ins GPRn:$rn),
     "INC", "$rn", [(set GPRn:$dst, (add GPRn:$rn, 1))]>;
    def DECRn : MCS51InstRn<1, (outs GPRn:$dst), (ins GPRn:$rn),
     "DEC", "$rn", [(set GPRn:$dst, (sub GPRn:$rn, 1))]>;
}

let Defs = [PSW], Constraints = "$dst = $ra" in {
    def ADDARn  : MCS51InstRn<2, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ADD", "$ra, $rn", [(set AREG:$dst, (add AREG:$ra, GPRn:$rn))]>;
    // FIXME: carry / borrow -- are addc and subc correct here?
    def ADDCARn : MCS51InstRn<3, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ADDC", "$ra, $rn", [(set AREG:$dst, (addc AREG:$ra, GPRn:$rn))]>;
    def SUBBARn : MCS51InstRn<9, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "SUBB", "$ra, $rn", [(set AREG:$dst, (subc AREG:$ra, GPRn:$rn))]>;
}

let Constraints = "$dst = $ra" in {
    def ORLARn  : MCS51InstRn<4, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ORL", "$ra, $rn", [(set AREG:$dst, (or AREG:$ra, GPRn:$rn))]>;
    def ANLARn  : MCS51InstRn<5, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "ANL", "$ra, $rn", [(set AREG:$dst, (and AREG:$ra, GPRn:$rn))]>;
    def XRLARn  : MCS51InstRn<6, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn),
     "XRL", "$ra, $rn", [(set AREG:$dst, (xor AREG:$ra, GPRn:$rn))]>;
}

// FIXME: pattern for swapping A, Rn?
def XCNARn  : MCS51InstRn<0xC, (outs AREG:$ra), (ins GPRn:$rn),
 "XCH", "$ra, $rn", []>;

def MOVARn  : MCS51InstRn<0xE, (outs AREG:$ra), (ins GPRn:$rn),
 "MOV", "$ra, $rn", [(set AREG:$ra, GPRn:$rn)]>;
def MOVRnA  : MCS51InstRn<0xF, (outs GPRn:$rn), (ins AREG:$ra),
 "MOV", "$rn, $ra", [(set GPRn:$rn, AREG:$ra)]>;
