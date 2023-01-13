#include "AST2IR/ast2ir.h"

#define FUNC_LIST_SIZE 10
std::string builtin_func_list[] = {
  "random",
  "zeros",
  "print"
};

bool isBuiltinFunc(std::string & func) {
  for (size_t i = 0; i < FUNC_LIST_SIZE; i++) {
    if (func == builtin_func_list[i]) {
      return true;
    }
  }
  return false;
}

// bool ASTConverterClassVisitor::VisitVarDecl(clang::VarDecl *vd) {
//   // llvm::errs() << vd->getType().getAsString() << "\n";
//   // llvm::errs() << vd->getName().str() << "\n";
//   return true;
// }

// bool ASTConverterClassVisitor::VisitMemberExpr(clang::MemberExpr *me) {
//   auto func_name = me->getMemberNameInfo().getAsString();
//   if(isBuiltinFunc(func_name)) {
//     llvm::errs() << "Builtin:";
//   }
//   llvm::errs() << me->getMemberNameInfo().getAsString() << "\n"; 
  
//   return true;
// }

