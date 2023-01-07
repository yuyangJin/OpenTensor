add_executable(
    OpenTensor

    opentensor.cpp
    )

set(LIBS
    IR
)

target_link_libraries(OpenTensor PRIVATE ${LIBS})