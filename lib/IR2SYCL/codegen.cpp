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

void CodeGenerator::generate(DataIRNode *dn) {}
void CodeGenerator::generate(CallIRNode *cn) {
  std::string &callee_name = cn->getCalleeFuncName();
  if (callee_name.compare("malloc") == 0) {
    _fs << "DATA_TYPE *" << 'A' << " = (DATA_TYPE *)malloc("
        << cn->getArgs().getArg(0) << " * sizeof(DATA_TYPE));" << std::endl;
  }
}
void CodeGenerator::generate(MemIRNode *mn) {}
void CodeGenerator::generate(TaskIRNode *tn) {}
void CodeGenerator::generate(EinsumTaskIRNode *etn) {}
void CodeGenerator::generate(CommIRNode *cn) {}
void CodeGenerator::generate(ParaIRNode *pn) {}
void CodeGenerator::generate(ForIRNode *fn) {}
void CodeGenerator::generate(BranchIRNode *bn) {}

void CodeGenerator::generate(std::string &file_name) {
  /** Create ofstream */
  _fs.open(file_name, std::ios::out);

  /** Generate SYCL code from IRGraph */
  std::vector<irnode_id_t> accessed_nodelist;
  std::vector<irnode_id_t> generating_nodelist;
  std::vector<irnode_id_t> waiting_nodelist;

  /** 1. insert all nodes with no in edges into generating_nodelist
   *  also insert them into accessed_nodelist */

  /** 2. while generating_nodelist is not empty */

  /**   2.1 generate node X in generating_nodelist */

  /**   2.2 check the dest nodes of node X's out edges,
   *    if dest nodes are accessed,
   *      then continue
   *    else,
   *      if dest nodes' input data node are ready,
   *        then insert them into generating_nodelist,
   *      else,
   *        insert them into waiting_nodelist,
   */

  /**   2.3 insert all dest nodes into accessed_nodelist */

  /**   2.4 check nodes in waiting node (here accessed nodelist is updated),
   *    if nodes' input data node are ready,
   *      then move it to generating nodelist from waiting nodelist
   *    else,
   *      then continue
   */
  /** end while*/
}