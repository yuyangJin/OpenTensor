#include "IR/irgraph.h"

IRGraphDumper::IRGraphDumper() { _fs.open("graph.dot", std::ios::out); }

IRGraphDumper::~IRGraphDumper() { _fs.close(); }

void IRGraphDumper::dumpGraph(IRGraph *graph) {
  _fs << "digraph G {" << std::endl;

  auto &nodes = graph->getNodes();

  for (long unsigned int i = 0; i < nodes.size(); i++) {
    // auto node = nodes[i];
    auto node = nodes[i].get();
    dump(node);
  }

  auto &edges = graph->getEdges();
  for (auto &edge : edges) {
    dumpEdge(edge);
  }

  _fs << "}" << std::endl;
}

void IRGraphDumper::dumpEdge(edge_t &edge) {
  _fs << "  " << edge.first << " -> " << edge.second << ";" << std::endl;
}
/**
 * @brief Standard output for
 *
 */
void IRGraphDumper::dumpNode(irnode_id_t id, std::string &name,
                             std::string &shape) {
  _fs << "  " << id << " [";
  _fs << "label=\"" << name << "\", ";
  _fs << "shape=" << shape;
  // _fs << ",style=filled,color=white";
  _fs << "];\n";
}

void IRGraphDumper::dumpNode(irnode_id_t id, std::string &name,
                             std::string &shape, int font_size) {
  _fs << "  " << id << " [";
  _fs << "label=\"" << name << "\", ";
  _fs << "shape=" << shape << ",";
  _fs << "fontsize=" << font_size;
  _fs << "];\n";
}

void IRGraphDumper::dumpRegion(irnode_id_t id, std::string &name,
                               IRNodeList *body, std::string &color) {
  _fs << "  subgraph cluster" << id << " {\n";
  _fs << "    label=\"" << name << "\";\n";
  _fs << "    color=" << color << ";\n";
  _fs << "    style=filled;\n";
  _fs << "    node [style=filled,color=white]\n";
  for (auto n : *body) {
    _fs << "    " << n << ";\n";
  }
  _fs << "  }\n";
}

void IRGraphDumper::dump(IRNode *n) {
  if (n == nullptr) {
    return;
  }
  auto node_type = n->getType();
  // dbg(n);
  // dbg(node_type);
  switch (node_type) {
  case irnode_type_t::IRNode_Data: {
    auto *dn = dynamic_cast<DataIRNode *>(n);
    dump(dn);
    break;
  }
  case irnode_type_t::IRNode_BinOp: {
    auto *bn = dynamic_cast<BinIRNode *>(n);
    dump(bn);
    break;
  }
  case irnode_type_t::IRNode_Call: {
    auto *cn = dynamic_cast<CallIRNode *>(n);
    dump(cn);
    break;
  }
  case irnode_type_t::IRNode_Mem: {
    auto *mn = dynamic_cast<MemIRNode *>(n);
    dump(mn);
    break;
  }
  case irnode_type_t::IRNode_Task: {
    auto *tn = dynamic_cast<TaskIRNode *>(n);
    dump(tn);
    break;
  }
  case irnode_type_t::IRNode_EinsumTask: {
    auto *etn = dynamic_cast<EinsumTaskIRNode *>(n);
    dump(etn);
    break;
  }
  case irnode_type_t::IRNode_Comm: {
    auto *cn = dynamic_cast<CommIRNode *>(n);
    dump(cn);
    break;
  }
  case irnode_type_t::IRNode_Parallel: {
    auto *pn = dynamic_cast<ParaIRNode *>(n);
    dump(pn);
    break;
  }
  case irnode_type_t::IRNode_For: {
    auto *fn = dynamic_cast<ForIRNode *>(n);
    dump(fn);
    break;
  }
  case irnode_type_t::IRNode_Branch: {
    auto *bn = dynamic_cast<BranchIRNode *>(n);
    dump(bn);
    break;
  }
  default:
    break;
  }
}

void IRGraphDumper::dump(DataIRNode *dn) {
  std::string shape = std::string("ellipse");
  std::string label = dn->getName();

  auto &data_shape = dn->getShape();
  auto num_dims = data_shape.getNumDims();
  if (num_dims > 0) {
    label += '<';
    for (size_t i = 0; i < num_dims - 1; i++) {
      label += std::to_string(data_shape.getDim(i)) + ", ";
    }
    label += std::to_string(data_shape.getDim(num_dims - 1));
    label += '>';
  }

  dumpNode(dn->getId(), label, shape, 9);
}

void IRGraphDumper::dump(BinIRNode *dn) {
  std::string shape = std::string("box");
  std::string label = dn->getOp();

  dumpNode(dn->getId(), label, shape, 9);
}

// void IRGraphDumper::dump(SliceIRNode *sn) {
//   std::string shape = std::string("box");
//   std::string label = sn->getSliceRange();
  
//   dumpNode(sn->getId(), label, shape, 9);
// }


void IRGraphDumper::dump(CallIRNode *cn) {
  std::string shape = std::string("box");
  std::string label = cn->getCalleeFuncName();
  label += '(';
  auto &args = cn->getArgs();
  auto num_args = args.getNumArgs();
  if (num_args > 0) {

    for (size_t i = 0; i < num_args - 1; i++) {
      label += args.getArg(i) + ", ";
    }
    label += args.getArg(num_args - 1);
  }
  label += ')';

  dumpNode(cn->getId(), label, shape, 10);
}
void IRGraphDumper::dump(MemIRNode *mn) {
  std::string shape = std::string("box");
  dumpNode(mn->getId(), mn->getAccessModeString(), shape);
}
void IRGraphDumper::dump(TaskIRNode *tn) {}
void IRGraphDumper::dump(EinsumTaskIRNode *etn) {
  std::string shape = std::string("octagon");
  std::string name = etn->getLHS() + " = (" + etn->getRHS() + ")";
  auto reduction_mode = etn->getReductionMode();
  auto type = reduction_mode.getReductionType();
  switch (type) {
  case SUM: {
    name += ".SUM(";
    break;
  }
  case MAX: {
    name += ".MAX(";
    break;
  }
  case MIN: {
    name += ".MIN(";
    break;
  }
  case AVG: {
    name += ".AVG(";
    break;
  }
  default:
    break;
  }
  auto num_dims = reduction_mode.getNumReductionDims();

  for (size_t i = 0; i < num_dims - 1; i++) {
    name += std::string(reduction_mode.getReductionDim(i)) + ',';
  }

  name += std::string(reduction_mode.getReductionDim(num_dims - 1)) + ')';

  dumpNode(etn->getId(), name, shape);
}
void IRGraphDumper::dump(CommIRNode *cn) {}
void IRGraphDumper::dump(ParaIRNode *pn) {
  std::string name = std::string("Parallel ( ");
  for (auto &dim : pn->getParaShape()._dims) {
    name += std::string(dim) + " ";
  }
  name += ")";
  std::string color = std::string("lightblue");
  dumpRegion(pn->getId(), name, pn->getBody(), color);
}
void IRGraphDumper::dump(ForIRNode *fn) {}
void IRGraphDumper::dump(BranchIRNode *bn) {}