# RUN: llvm-mc -triple mcs51 -show-encoding %s | FileCheck %s

ADD  A, R0
ADDC A, R1
ADD  A, R2

# CHECK: .text
# CHECK: ADD     A, R0                           # encoding: [0x28]
# CHECK: ADDC    A, R1                           # encoding: [0x39]
# CHECK: ADD     A, R2                           # encoding: [0x2a]
