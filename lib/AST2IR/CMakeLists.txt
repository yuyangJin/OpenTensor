message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using ClangConifg.cmake in ${CLANG_CMAKE_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti") # We cannot open it because dynamic_cast needs it

set(LLVM_LINK_COMPONENTS
  Option
  Support
)

set(DEP_CLANG_LIBS
    clangAST
    clangBasic
    clangParse
    clangFrontend
    clangTooling
    clangAnalysis
    clangFrontendTool
    clangASTMatchers
    clangDynamicASTMatchers
)

set(PF_LIBS
    PowerFusion
)

add_library(AST2IR
    SHARED
    ast2ir.cpp
    expr_pat.cpp
)


target_link_libraries(AST2IR ${DEP_CLANG_LIBS} ${PF_LIBS})
