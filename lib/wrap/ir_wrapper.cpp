#include <pybind11/pybind11.h>
#include "IR/irgraph.h"
#include "IR/irnode.h"

// namespace py = pybind11;

using namespace pybind11::literals;

PYBIND11_MODULE(TensorIR, m) {
    // pybind11::module m("TensorIR", "Tensor IR plugin");

    pybind11::enum_<irnode_type_t>(m, "irnode_type_t")
        .value("IRNode_Data", irnode_type_t::IRNode_Data)
        .value("IRNode_Call", irnode_type_t::IRNode_Call)
        .value("IRNode_Mem", irnode_type_t::IRNode_Mem)
        .value("IRNode_Task", irnode_type_t::IRNode_Task)
        .value("IRNode_For", irnode_type_t::IRNode_For)
        .value("IRNode_Branch", irnode_type_t::IRNode_Branch);

    pybind11::class_<IRNode> (m, "IRNode")
        .def(pybind11::init<irnode_type_t>());

    pybind11::class_<DataShape> (m, "DataShape")
        .def(pybind11::init<>())
        .def("add_dim", &DataShape::addDim);

    pybind11::class_<DataIRNode, IRNode> (m, "DataIRNode")
        .def(pybind11::init<std::string &, DataShape>());

    pybind11::class_<BinIRNode, IRNode> (m, "BinIRNode")
        .def(pybind11::init<std::string &>());
    
    pybind11::class_<ArgList> (m, "ArgList")
        .def(pybind11::init<>())
        .def("add_arg", &ArgList::addArg);

    pybind11::class_<CallIRNode, IRNode> (m, "CallIRNode")
        .def(pybind11::init<std::string &, std::string &, ArgList>());

    pybind11::class_<IRGraph> (m, "IRGraph")
        .def(pybind11::init<>())
        .def("add_node", &IRGraph::addNode)
        .def("add_datanode", &IRGraph::addDataNode)
        .def("add_binnode", &IRGraph::addBinNode)
        .def("add_edge", &IRGraph::addEdge);


    pybind11::class_<IRGraphDumper> (m, "IRGraphDumper")
        .def(pybind11::init<>())
        .def("dump", &IRGraphDumper::dumpGraph);


    // m.def("", &add, "A function which adds two numbers",
    //       "i"_a=1, "j"_a=2);

    // return m.ptr();
}