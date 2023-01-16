#include "IR2SYCL/codegen.h"

void CodeGenerator::generate(IRNode *n) {
  if (n == nullptr) {
    return;
  }
  auto node_type = n->getType();
  switch (node_type) {
  case IRNode_Data: {
    auto *dn = dynamic_cast<DataIRNode *>(n);
    generate(dn);
    break;
  }
  case IRNode_Call: {
    auto *cn = dynamic_cast<CallIRNode *>(n);
    generate(cn);
    break;
  }
  case IRNode_Mem: {
    auto *mn = dynamic_cast<MemIRNode *>(n);
    generate(mn);
    break;
  }
  case IRNode_Task: {
    auto *tn = dynamic_cast<TaskIRNode *>(n);
    generate(tn);
    break;
  }
  case IRNode_EinsumTask: {
    auto *etn = dynamic_cast<EinsumTaskIRNode *>(n);
    generate(etn);
    break;
  }
  case IRNode_Comm: {
    auto *cn = dynamic_cast<CommIRNode *>(n);
    generate(cn);
    break;
  }
  case IRNode_Parallel: {
    auto *pn = dynamic_cast<ParaIRNode *>(n);
    generate(pn);
    break;
  }
  case IRNode_For: {
    auto *fn = dynamic_cast<ForIRNode *>(n);
    generate(fn);
    break;
  }
  case IRNode_Branch: {
    auto *bn = dynamic_cast<BranchIRNode *>(n);
    generate(bn);
    break;
  }
  default:
    break;
  }
}

void CodeGenerator::generate(DataIRNode *dn) {
  std::string &tensor_name = dn->getName();
  auto &tensor_shape = dn->getShape();
  auto num_dims = tensor_shape.getNumDims();
  dbg(tensor_name, num_dims);
  if (num_dims > 0) {
    _fs << "sycl::buffer<DATA_TYPE, " << num_dims << "> " << tensor_name
        << "_buffer(_" << tensor_name << ", sycl::range<" << num_dims << ">(";
    for (size_t i = 0; i < num_dims - 1; i++) {
      _fs << tensor_shape.getDim(i) << ", ";
    }
    _fs << tensor_shape.getDim(num_dims - 1);
    _fs << "));" << std::endl;
  }
}
void CodeGenerator::generate(CallIRNode *cn) {
  std::string &callee_name = cn->getCalleeFuncName();
  auto &args = cn->getArgs();
  dbg(callee_name);
  if (callee_name.compare("malloc") == 0) {
    _fs << "DATA_TYPE *_" << cn->getObjName() << " = (DATA_TYPE *)malloc("
        << args.getArg(0) << " * sizeof(DATA_TYPE));" << std::endl;
  } else {
    _fs << callee_name << "(" << cn->getObjName();
    auto num_args = args.getNumArgs();
    for (size_t i = 0; i < num_args; i++) {
      _fs << ", " << args.getArg(i);
    }
    _fs << ");" << std::endl;
  }
}
void CodeGenerator::generate(MemIRNode *mn) {
  auto access_mode = mn->getAccessMode();
  if (access_mode == READ || access_mode == READ_WRITE) {
    auto *src_nodes = _graph->getSrcNodes(mn->getId());
    if (src_nodes->size() == 1) {
      auto *tensor_node =
          dynamic_cast<DataIRNode *>(_graph->getNode((*src_nodes)[0]));
      std::string &tensor_name = tensor_node->getName();
      _fs << "auto " << tensor_name << " = " << tensor_name
          << "_buffer.get_access<sycl::access::mode::"
          << mn->getAccessModeString() << ">(cgh);" << std::endl;
    }
  } else if (access_mode == WRITE) {
    auto *dest_nodes = _graph->getDestNodes(mn->getId());
    if (dest_nodes->size() == 1) {
      auto *tensor_node =
          dynamic_cast<DataIRNode *>(_graph->getNode((*dest_nodes)[0]));
      std::string &tensor_name = tensor_node->getName();
      _fs << "auto " << tensor_name << " = " << tensor_name
          << "_buffer.get_access<sycl::access::mode::"
          << mn->getAccessModeString() << ">(cgh);" << std::endl;
    }
  }
}
void CodeGenerator::generate(TaskIRNode *tn) {}
void CodeGenerator::generate(EinsumTaskIRNode *etn) {
  auto &reduction_mode = etn->getReductionMode();
  auto num_dims = reduction_mode.getNumReductionDims();
  auto reduction_type = reduction_mode.getReductionType();
  dbg(etn->getLHS(), etn->getRHS());
  for (size_t i = 0; i < num_dims; i++) {
    std::string &dim_name = reduction_mode.getReductionDim(i);
    _fs << "for(size_t " << dim_name << " = 0; " << dim_name << " < 4; "
        << dim_name << "++) {" << std::endl;
  }
  _fs << "  " << etn->getLHS() << " ";
  if (reduction_type == SUM) {
    _fs << "+=";
  }
  _fs << " " << etn->getRHS() << ";" << std::endl;
  for (size_t i = 0; i < num_dims; i++) {
    _fs << "}" << std::endl;
  }
}
void CodeGenerator::generate(CommIRNode *cn) {}
void CodeGenerator::generate(ParaIRNode *pn) {}
void CodeGenerator::generate(ForIRNode *fn) {}
void CodeGenerator::generate(BranchIRNode *bn) {}

bool CodeGenerator::isNodesAccessed(IRNodeList *src_nodes) {
  bool all_src_nodes_are_accessed = true;
  for (auto src_node_id : *src_nodes) {
    IRNodeList::iterator it =
        find(_accessed_nodelist.begin(), _accessed_nodelist.end(), src_node_id);
    if (it == _accessed_nodelist.end()) { // has not accessed
      all_src_nodes_are_accessed = false;
      break;
    }
  }
  return all_src_nodes_are_accessed;
}

bool CodeGenerator::isSrcNodesAccessed(irnode_id_t node_id) {
  auto *src_nodes = _graph->getSrcNodes(node_id);
  return isNodesAccessed(src_nodes);
}

void CodeGenerator::generate(std::string &file_name) {
  /** Create ofstream */
  _fs.open(file_name, std::ios::out);

  /** Generate SYCL code from IRGraph */
  _fs << "// Creating SYCL queue" << std::endl
      << "sycl::queue Queue;" << std::endl;

  /** 1. insert all nodes with no in edges into generating nodelist
   *  also insert them into accessed_nodelist */
  auto *nodes_without_in_edge = _graph->getNodesWithoutInEdges();
  dbg(*nodes_without_in_edge);
  for (auto node_id : *nodes_without_in_edge) {
    _generating_nodelist.emplace_back(node_id);
    _accessed_nodelist.emplace_back(node_id);
  }

  /** 2. while generating nodelist is not empty */
  while (!_generating_nodelist.empty()) {
    dbg(_generating_nodelist);
    /**   2.1 generate node X in generating nodelist */
    auto node_id = _generating_nodelist.front();
    _generating_nodelist.erase(remove(_generating_nodelist.begin(),
                                      _generating_nodelist.end(), node_id),
                               _generating_nodelist.end());
    auto *node = _graph->getNode(node_id);
    dbg(node_id);
    generate(node);

    /**   2.2 check the dest nodes of node X's out edges,
     *    if dest nodes are accessed,
     *      then continue
     *    else,
     *      if dest nodes' input data(src ) node are accessed,
     *        then insert dest nodes into generating nodelist,
     *      else,
     *        insert dest nodes into waiting nodelist,
     */
    auto *dest_nodes = _graph->getDestNodes(node_id);
    dbg(*dest_nodes);
    for (auto dest_node_id : *dest_nodes) {
      IRNodeList::iterator it = find(_accessed_nodelist.begin(),
                                     _accessed_nodelist.end(), dest_node_id);
      if (it != _accessed_nodelist.end()) { // has accessed
        continue;
      } else {
        if (isSrcNodesAccessed(dest_node_id)) {
          _generating_nodelist.emplace_back(dest_node_id);
        } else {
          _waiting_nodelist.emplace_back(dest_node_id);
        }
        /**   2.3 insert all dest nodes into accessed_nodelist */
        _accessed_nodelist.emplace_back(dest_node_id);
      }
    }

    /**   2.4 check nodes in waiting node (here accessed nodelist is updated),
     *    if nodes' input data node (src node) are ready (accessed),
     *      then move it to generating nodelist from waiting nodelist
     *    else,
     *      then continue
     */
    IRNodeList::iterator it = _waiting_nodelist.begin();
    while (it != _waiting_nodelist.end()) {
      auto waiting_node_id = *it;
      if (isSrcNodesAccessed(waiting_node_id)) {
        it = remove(_waiting_nodelist.begin(), _waiting_nodelist.end(),
                    waiting_node_id);
        _waiting_nodelist.erase(it, _waiting_nodelist.end());
        _generating_nodelist.emplace_back(waiting_node_id);
        // do not ++it, because remove return the iterator of next element;
      } else {
        ++it;
      }
    }

  } /** end while*/
}