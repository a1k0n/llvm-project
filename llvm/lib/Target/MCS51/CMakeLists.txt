set(LLVM_TARGET_DEFINITIONS MCS51.td)

tablegen(LLVM MCS51GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MCS51GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MCS51GenMCCodeEmitter.inc -gen-emitter)

add_public_tablegen_target(MCS51CommonTableGen)

add_llvm_target(MCS51CodeGen
  MCS51TargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
