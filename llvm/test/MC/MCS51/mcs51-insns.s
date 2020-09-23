# RUN: llvm-mc -triple mcs51 -show-encoding %s | FileCheck %s

# CHECK: .text
CLR  A
# CHECK: CLR  A     ; encoding: [0xe4]
ADD  A, #4
# CHECK: ADD  A, #4 ; encoding: [0x24,0x04]
ADD  A, R0
# CHECK: ADD  A, R0 ; encoding: [0x28]
ADDC A, R1
# CHECK: ADDC A, R1 ; encoding: [0x39]
ADD  A, R2
# CHECK: ADD  A, R2 ; encoding: [0x2a]
XRL  A, R7
# CHECK: XRL  A, R7 ; encoding: [0x6f]
MOV  R2, A
# CHECK: MOV  R2, A ; encoding: [0xfa]
MOV  A, R3
# CHECK: MOV  A, R3 ; encoding: [0xeb]
INC  @R0
# CHECK: INC  @R0   ; encoding: [0x06]
NOP
# CHECK: NOP        ; encoding: [0x00]
LJMP 0x1234
# CHECK: LJMP 4660  ; encoding: [0x02,0x12,0x34]
RET
# CHECK: RET        ; encoding: [0x22]

