; RUN: llvm-mc -triple mcs51 -show-encoding %s | FileCheck -check-prefixes=CHECK,CHECK-INST %s
; RUN: llvm-mc -triple mcs51 -filetype obj < %s | llvm-objdump -d - | FileCheck -check-prefixes=CHECK,CHECK-OBJDUMP %s
; RUN: llvm-mc -triple mcs51 -filetype obj < %s | llvm-objdump -r - | FileCheck -check-prefix=RELOC %s

; CHECK: .text
CLR  A
; CHECK: CLR  A
; CHECK-INST: ; encoding: [0xe4]
ADD  A, #4
; CHECK: ADD  A, #4
; CHECK-INST: ; encoding: [0x24,0x04]
ADD  A, R0
; CHECK: ADD  A, R0
; CHECK-INST: ; encoding: [0x28]
ADDC A, R1
; CHECK: ADDC A, R1
; CHECK-INST: ; encoding: [0x39]
ADD  A, R2
; CHECK: ADD  A, R2
; CHECK-INST: ; encoding: [0x2a]
XRL  A, R7
; CHECK: XRL  A, R7
; CHECK-INST: ; encoding: [0x6f]
MOV  R2, A
; CHECK: MOV  R2, A
; CHECK-INST: ; encoding: [0xfa]
MOV  A, R3
; CHECK: MOV  A, R3
; CHECK-INST: ; encoding: [0xeb]
INC  R0
; CHECK: INC  R0
; CHECK-INST: ; encoding: [0x08]
INC  @R0
; CHECK: INC  @R0
; CHECK-INST: ; encoding: [0x06]
NOP
; CHECK: NOP
; CHECK-INST: ; encoding: [0x00]
LJMP 0x1234
; CHECK: LJMP 0x1234
; CHECK-INST: ; encoding: [0x02,0x12,0x34]
RET
; CHECK: RET
; CHECK-INST: ; encoding: [0x22]

MOV A, #0x12
; CHECK: MOV A, #18
; CHECK-INST: ; encoding: [0x74,0x12]
XRL A, #0x80
; CHECK: XRL A, #128
; CHECK-INST: ; encoding: [0x64,0x80]
ORL A, #1
; CHECK: ORL A, #1
; CHECK-INST: ; encoding: [0x44,0x01]
XCH A, R7
; CHECK: XCH A, R7
; CHECK-INST: ; encoding: [0xcf]
INC 0x45
; CHECK: INC 69
; CHECK-INST: ; encoding: [0x05,0x45]
DEC 0x80
; CHECK: DEC 128
; CHECK-INST: ; encoding: [0x15,0x80]
XRL A, 0x45
; CHECK: XRL A, 69
; CHECK-INST: ; encoding: [0x65,0x45]
XRL A, @R1
; CHECK: XRL A, @R1
; CHECK-INST: ; encoding: [0x67]

.LBB0:
MOV A, #.LBB0
; CHECK-INST: MOV     A, #.LBB0 ; encoding: [0x74,A]
; CHECK-INST:                   ;   fixup A - offset: 1, value: .LBB0, kind: FK_Data_1
; CHECK-OBJDUMP: 74 00         MOV     A, #0
LJMP .LBB0
; CHECK-INST: LJMP    .LBB0     ; encoding: [0x02,A,A]
; CHECK-INST:                   ;   fixup A - offset: 1, value: .LBB0, kind: FK_Data_2
; CHECK-OBJDUMP: 02 00 00      LJMP    0x0
SJMP .LBB0
; CHECK-INST: SJMP    .LBB0     ; encoding: [0x80,A]
; CHECK-INST:                   ;   fixup A - offset: 1, value: .LBB0, kind: FK_PCRel_1
; CHECK-OBJDUMP: 80 f9         SJMP    .-7

; RELOC: RELOCATION RECORDS FOR [.text]:
; RELOC: OFFSET   TYPE                     VALUE
; RELOC: 0000001f R_MCS51_ADDR8            .text+0x1e
; RELOC: 00000021 R_MCS51_ADDR16           .text+0x1e


MOV R0, R7
MOV R1, R6
MOV A, @R0
MOV @R1, A
MOV A, 0xBA
MOV 0xBD, A
; CHECK-INST:        MOV     R0, R7                          ; encoding: [0xa8,0x07]
; CHECK-INST:        MOV     R1, R6                          ; encoding: [0xa9,0x06]
; CHECK-INST:        MOV     A, @R0                          ; encoding: [0xe6]
; CHECK-INST:        MOV     @R1, A                          ; encoding: [0xf7]
; CHECK-INST:        MOV     A, 186                          ; encoding: [0xe5,0xba]
; CHECK-INST:        MOV     189, A                          ; encoding: [0xf5,0xbd]
; CHECK-OBJDUMP:      25: a8 07         MOV     R0, R7
; CHECK-OBJDUMP:      27: a9 06         MOV     R1, R6
; CHECK-OBJDUMP:      29: e6            MOV     A, @R0
; CHECK-OBJDUMP:      2a: f7            MOV     @R1, A
; CHECK-OBJDUMP:      2b: e5 ba         MOV     A, 186
; CHECK-OBJDUMP:      2d: f5 bd         MOV     189, A
