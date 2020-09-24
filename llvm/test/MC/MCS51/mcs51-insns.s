# RUN: llvm-mc -triple mcs51 -show-encoding %s | FileCheck -check-prefixes=CHECK,CHECK-INST %s
# RUN: llvm-mc -triple mcs51 -filetype obj < %s | llvm-objdump -d - | FileCheck -check-prefix=CHECK %s

# CHECK: .text
CLR  A
# CHECK: CLR  A
# CHECK-INST: ; encoding: [0xe4]
ADD  A, #4
# CHECK: ADD  A, #4
# CHECK-INST: ; encoding: [0x24,0x04]
ADD  A, R0
# CHECK: ADD  A, R0
# CHECK-INST: ; encoding: [0x28]
ADDC A, R1
# CHECK: ADDC A, R1
# CHECK-INST: ; encoding: [0x39]
ADD  A, R2
# CHECK: ADD  A, R2
# CHECK-INST: ; encoding: [0x2a]
XRL  A, R7
# CHECK: XRL  A, R7
# CHECK-INST: ; encoding: [0x6f]
MOV  R2, A
# CHECK: MOV  R2, A
# CHECK-INST: ; encoding: [0xfa]
MOV  A, R3
# CHECK: MOV  A, R3
# CHECK-INST: ; encoding: [0xeb]
INC  R0
# CHECK: INC  R0
# CHECK-INST: ; encoding: [0x08]
INC  @R0
# CHECK: INC  @R0
# CHECK-INST: ; encoding: [0x06]
NOP
# CHECK: NOP
# CHECK-INST: ; encoding: [0x00]
LJMP 0x1234
# CHECK: LJMP 4660
# CHECK-INST: ; encoding: [0x02,0x12,0x34]
RET
# CHECK: RET
# CHECK-INST: ; encoding: [0x22]

MOV A, #0x12
# CHECK: MOV A, #18
# CHECK-INST: ; encoding: [0x74,0x12]
XRL A, #0x80
# CHECK: XRL A, #128
# CHECK-INST: ; encoding: [0x64,0x80]
ORL A, #1
# CHECK: ORL A, #1
# CHECK-INST: ; encoding: [0x44,0x01]
XCH A, R7
# CHECK: XCH A, R7
# CHECK-INST: ; encoding: [0xcf]
INC 0x45
# CHECK: INC 69
# CHECK-INST: ; encoding: [0x05,0x45]
DEC 0x80
# CHECK: DEC 128
# CHECK-INST: ; encoding: [0x15,0x80]
XRL A, 0x45
# CHECK: XRL A, 69
# CHECK-INST: ; encoding: [0x65,0x45]
XRL A, @R1
# CHECK: XRL A, @R1
# CHECK-INST: ; encoding: [0x67]
