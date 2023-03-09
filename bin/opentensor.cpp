#include "AST2IR/ast2ir.h"
// #include "IR/irgraph.h"
// #include "IR/irnode.h"
// #include "IR2SYCL/codegen.h"

int main(int argc, const char **argv) {
  auto ast_converter = new ASTConverter(argc, argv);
  ast_converter->generate();
  // auto irgraph = ast_converter->getIRGraph();
  // auto dumper = new IRGraphDumper();
  // dumper->dump(irgraph);
  // auto cg = new CodeGenerator(irgraph);
  // std::string file_name("sycl.cpp");
  // cg->generate(file_name);
}