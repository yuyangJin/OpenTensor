#include "AST2IR/ast2ir.h"
#include "IR/irgraph.h"
#include "IR/irnode.h"

int main(int argc, const char **argv) {
  auto ast_converter = new ASTConverter(argc, argv);
  auto irgraph = ast_converter->getIRGraph();
  auto dumper = new IRGraphDumper();
  dumper->dump(irgraph);
  delete dumper;
}