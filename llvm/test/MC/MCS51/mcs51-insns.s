# RUN: llvm-mc -triple mcs51 -show-encoding %s | FileCheck %s

ADD  A, R0
ADDC A, R1
ADD  A, R2
XRL  A, R7
MOV  R2, A
MOV  A, R3
NOP
RET

# CHECK: .text
# CHECK: ADD     A, R0                           # encoding: [0x28]
# CHECK: ADDC    A, R1                           # encoding: [0x39]
# CHECK: ADD     A, R2                           # encoding: [0x2a]
# CHECK: XRL     A, R7                           # encoding: [0x6f]
# CHECK: MOV     R2, A                           # encoding: [0xfa]
# CHECK: MOV     A, R3                           # encoding: [0xeb]
# CHECK: NOP                                     # encoding: [0x00]
# CHECK: RET                                     # encoding: [0x22]

