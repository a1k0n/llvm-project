//===-- MCS51RegisterInfo.td - MCS51 Register defs -------------*- tablegen -*-===//

let Namespace = "MCS51" in {
class MCS51Reg<bits<4> num, bits<8> memaddr, string name, list<Register> subregs = [], list<string> altNames = []>
  : RegisterWithSubRegs<name, subregs>
{
  let HWEncoding{3-0} = num;
  field bits<8> MemAddr = memaddr;
  let SubRegs = subregs;
  let AltNames = altNames;
}
}  // Namespace = "MCS51"

def R0 : MCS51Reg<0, 0, "R0", [], []>, DwarfRegNum<[0]>;
def R1 : MCS51Reg<1, 1, "R1", [], []>, DwarfRegNum<[1]>;
def R2 : MCS51Reg<2, 2, "R2", [], []>, DwarfRegNum<[2]>;
def R3 : MCS51Reg<3, 3, "R3", [], []>, DwarfRegNum<[3]>;
def R4 : MCS51Reg<4, 4, "R4", [], []>, DwarfRegNum<[4]>;
def R5 : MCS51Reg<5, 5, "R5", [], []>, DwarfRegNum<[5]>;
def R6 : MCS51Reg<6, 6, "R6", [], []>, DwarfRegNum<[6]>;
def R7 : MCS51Reg<7, 7, "R7", [], []>, DwarfRegNum<[7]>;

def A : MCS51Reg<8, 0xe0, "A", [], ["ACC"]>, DwarfRegNum<[8]>;
def B : MCS51Reg<9, 0xf0, "B", [], []>, DwarfRegNum<[9]>;
def PSW : MCS51Reg<10, 0xd0, "PSW", [], []>, DwarfRegNum<[10]>;

def DPL : MCS51Reg<11, 0x82, "DPL", [], []>, DwarfRegNum<[11]>;
def DPH : MCS51Reg<12, 0x83, "DPH", [], []>, DwarfRegNum<[12]>;

let SubRegIndices = [SubRegIndex<8>, SubRegIndex<8, 8>], CoveredBySubRegs = 1 in {
  def DPTR : MCS51Reg<11, 0x82, "DPTR", [DPL, DPH], []>, DwarfRegNum<[11]>;

  def R0R1 : MCS51Reg<0, 0, "R0R1", [R0, R1], []>, DwarfRegNum<[0]>;
  def R2R3 : MCS51Reg<2, 2, "R2R3", [R2, R3], []>, DwarfRegNum<[2]>;
  def R4R5 : MCS51Reg<4, 4, "R4R5", [R4, R5], []>, DwarfRegNum<[4]>;
  def R6R7 : MCS51Reg<6, 6, "R6R7", [R6, R7], []>, DwarfRegNum<[6]>;
}

// TODO: once codegen is implemented, registers should be listed in an order
// reflecting the preferred register allocation sequence.

def GPR8 : RegisterClass<"MCS51", [i8], 8,
  (add A, R0, R1, R2, R3, R4, R5, R6, R7, B, DPL, DPH)>;

def PTR16: RegisterClass<"MCS51", [i16], 8,
  (add DPTR)>;


