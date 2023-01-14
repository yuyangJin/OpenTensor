cmake_minimum_required(VERSION 3.16)

project(opentensor CXX C)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  message(FATAL_ERROR "Do not support non-Linux now!")
endif()

# TODO: remove it in the future
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Set build type to DEBUG" FORCE)
message(AUTHOR_WARNING "Overwrite build type to DEBUG for development")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

option(ENABLE_TEST "Enable test" OFF)
option(ENABLE_SANITIZE "Enable sanitize" ON)

# set_property(TARGET opentensor PROPERTY CXX_STANDARD 17)
# set_property(TARGET opentensor PROPERTY CXX_STANDARD_REQUIRED ON)
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#   if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0.0")
#     message(FATAL_ERROR "Insufficient GCC version (${CMAKE_CXX_COMPILER_VERSION}), need > 7.0.0")
#   endif()
# else()
#   message(FATAL_ERROR "Do not support cxx compiler for ${CMAKE_CXX_COMPILER_ID}, choose GNU please")
# endif()

add_compile_options(
  -Wall -Wno-missing-braces
  #-fno-rtti # We cannot open it because dynamic_cast needs it
  -fno-omit-frame-pointer -fstack-protector-all
  -rdynamic
)

if (ENABLE_SANITIZE) 
  add_compile_options(
    $<$<CONFIG:DEBUG>:-fsanitize=address -g>
  )
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third_party/dbg)


add_subdirectory(lib)
add_subdirectory(bin)

if (ENABLE_TEST)
  # List what we want to test
#   add_subdirectory(test)
endif()