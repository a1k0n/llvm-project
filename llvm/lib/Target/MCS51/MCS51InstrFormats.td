class MCS51Inst<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
  : Instruction {
  let Namespace = "MCS51";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;

  field bits<24> SoftFail = 0;
}

// Using the Ghidra 8051 SLEIGH definitions here
class MCS51Inst8<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst<outs, ins, opcodestr, argstr, pattern> {
  field bits<8> Inst;
  let Size = 1;
}

class MCS51InstNoArgs<bits<8> op, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst8<outs, ins, opcodestr, argstr, pattern> {
  let Inst = op;
}

class MCS51InstRn<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                  : MCS51Inst8<outs, ins, opcodestr, argstr, pattern> {
  bits<3> rn;

  let Inst{7-4} = ophi;
  let Inst{2-0} = rn;
  let Inst{3} = 1;  // "rnfill"
}

class MCS51InstRi<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                  : MCS51Inst8<outs, ins, opcodestr, argstr, pattern> {
  bits<1> ri;

  let Inst{7-4} = ophi;
  let Inst{0} = ri;
  let Inst{3-1} = 3;  // "rifill"
}