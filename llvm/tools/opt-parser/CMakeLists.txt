set(LLVM_LINK_COMPONENTS
  Core
  IRReader
  support
  )

add_llvm_tool(opt-parser
  opt-parser.cpp

  DEPENDS
  intrinsics_gen
  SUPPORT_PLUGINS
  )
export_executable_symbols_for_plugins(opt-parser)
