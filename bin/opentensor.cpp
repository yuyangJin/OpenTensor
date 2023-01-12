#include "AST2IR/ast2ir.h"
#include "IR/irnode.h"
#include "IR/irgraph.h"

int main (int argc, const char** argv) {
    auto ast_converter = new ASTConverter(argc, argv);
    auto irgraph = ast_converter->getIRGraph();
}