pybind11_add_module(TensorIR ir_wrapper.cpp ${PROJECT_SOURCE_DIR}/lib/IR/irgraph.cpp ${PROJECT_SOURCE_DIR}/lib/IR/irnode.cpp ${PROJECT_SOURCE_DIR}/lib/IR/irgraph_dumper.cpp)

# message(NOTICE "${CMAKE_CURRRENT_BINARY_DIR}")
# set(ENV{PYTHONPATH} "${CMAKE_CURRRENT_BINARY_DIR}/")

# export PYTHONPATH="${PYTHONPATH}:${CMAKE_CURRRENT_BINARY_DIR}/"