// MCS51 8-bit C return-value convention

// This calling convention is completely made up and arbitrary and based upon
// very little real-world input; please suggest improvements
def RetCC_MCS51 : CallingConv<[
  CCIfType<[i1], CCAssignToReg<[CF]>>,
  CCIfType<[i8], CCAssignToReg<[R7, R6, R5, R4, R3, R2]>>,
  CCIfType<[i16], CCAssignToReg<[R6R7, R4R5, R2R3]>>
]>;

def CC_MCS51 : CallingConv<[
  CCIfType<[i1], CCAssignToReg<[CF]>>,
  CCIfType<[i8], CCAssignToReg<[R7, R6, R5, R4, R3, R2, R1, R0, DPL, DPH, B, ACC]>>,
  CCIfType<[i16], CCAssignToReg<[R6R7, R4R5, R2R3, R0R1, DPTR]>>
]>;

def CSR : CalleeSavedRegs<(add R1, R2, R3, R4, R5, R6, DPL, DPH, B)>;

// interrupts will need to save everything of course, but they can use an alternate register bank
// this will be taken care of in the prologue/epilogue probably