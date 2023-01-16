#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "IR/irgraph.h"

class CodeGenerator {
public:
  CodeGenerator(IRGraph *graph) : _graph(graph) {}
  void generate(std::string &file_name);

private:
  IRGraph *_graph;
  std::ofstream _fs;
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
};

#endif // CODEGEN_H_