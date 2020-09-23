include "MCS51InstrFormats.td"

let Defs = [PSW], Constraints = "$dst = $ra" in {
    def ADDARn  : MCS51InstRn<0b0010, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn), "ADD", "$ra, $rn", [(set AREG:$dst, (add AREG:$ra, GPRn:$rn))]>;
    def ADDCARn : MCS51InstRn<0b0011, (outs AREG:$dst), (ins AREG:$ra, GPRn:$rn), "ADDC", "$ra, $rn", [(set AREG:$dst, (addc AREG:$ra, GPRn:$rn))]>;
}