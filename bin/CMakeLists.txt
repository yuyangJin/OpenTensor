include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

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

set(LIBS
    AST2IR
    IR
    IR2SYCL
)

add_executable(
    OpenTensor
    opentensor.cpp
)

target_link_libraries(OpenTensor PRIVATE ${LIBS} ${DEP_CLANG_LIBS})