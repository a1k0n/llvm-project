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
class Inst8<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst<outs, ins, opcodestr, argstr, pattern> {
  field bits<8> Inst;
  let Size = 1;
}

class MCS51InstAddr16<bits<8> op, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst<outs, ins, opcodestr, argstr, pattern> {
  field bits<24> Inst;
  bits<16> addr16;
  let Size = 3;

  // immediate addresses are in big endian order, but the rest of the CPU acts
  // like little-endian ... so I've opted to tell LLVM it's little-endian and
  // will swap these here.
  let Inst{7-0} = op;
  let Inst{15-8} = addr16{15-8};
  let Inst{23-16} = addr16{7-0};
}

class InsImm8<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst<outs, ins, opcodestr, argstr, pattern> {
  field bits<16> Inst;
  bits<8> imm8;
  let Size = 2;

  let Inst{7-4} = ophi;
  let Inst{3-0} = 4;
  let Inst{15-8} = imm8;
}

class InsDir8<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : MCS51Inst<outs, ins, opcodestr, argstr, pattern> {
  field bits<16> Inst;
  bits<8> a8;
  let Size = 2;

  let Inst{7-4} = ophi;
  let Inst{3-0} = 5;
  let Inst{15-8} = a8;
}

class InstNoArgs<bits<8> op, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                 : Inst8<outs, ins, opcodestr, argstr, pattern> {
  let Inst = op;
}

class InsARn<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                  : Inst8<outs, ins, opcodestr, argstr, pattern> {
  bits<3> rn;

  let Inst{7-4} = ophi;
  let Inst{2-0} = rn;
  let Inst{3} = 1;  // "rnfill"
}

class InsARi<bits<4> ophi, dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
                  : Inst8<outs, ins, opcodestr, argstr, pattern> {
  bits<1> ri;

  let Inst{7-4} = ophi;
  let Inst{0} = ri;
  let Inst{3-1} = 3;  // "rifill"
}