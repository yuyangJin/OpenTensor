#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "IR/irgraph.h"

#include <algorithm>

class CodeGenerator {
private:
  IRGraph *_graph;
  std::ofstream _fs;
  IRNodeList _accessed_nodelist;
  IRNodeList _generating_nodelist;
  IRNodeList _waiting_nodelist;

  void generate(IRNode *n);
  void generate(DataIRNode *dn);
  void generate(CallIRNode *cn);
  void generate(MemIRNode *mn);
  void generate(TaskIRNode *tn);
  void generate(EinsumTaskIRNode *etn);
  void generate(CommIRNode *cn);
  void generate(ParaIRNode *pn);
  void generate(ForIRNode *fn);
  void generate(BranchIRNode *bn);
  bool isNodesAccessed(IRNodeList *src_nodes);
  bool isSrcNodesAccessed(irnode_id_t node_id);

public:
  CodeGenerator(IRGraph *graph) : _graph(graph) {}
  void generate(std::string &file_name);
};

#endif // CODEGEN_H_