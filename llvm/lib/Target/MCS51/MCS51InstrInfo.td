include "MCS51InstrFormats.td"

let Uses = [A], Defs = [A, PSW] in {
    def ADDARn  : MCS51InstRn<0b0010, (outs), (ins GPR8:$rn), "ADD", "A, $rn", [(set A, (add A, GPR8:$rn))]>;
    def ADDCARn : MCS51InstRn<0b0011, (outs), (ins GPR8:$rn), "ADDC", "A, $rn", [(set A, (addc A, GPR8:$rn))]>;
}