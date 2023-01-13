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

ASTConverterClassConsumer::ASTConverterClassConsumer(clang::CompilerInstance *CI) {
  match_finder.addMatcher(clang::ast_matchers::varDecl(clang::ast_matchers::isExpansionInMainFile()).bind("VarDecl_in_main"), &handler);
  const auto matching_node = clang::ast_matchers::functionDecl(clang::ast_matchers::isExpansionInMainFile()).bind("fn");
  match_finder.addMatcher(matching_node, &handler);
}

void ASTConverterCallback::run(const clang::ast_matchers::MatchFinder::MatchResult &Result)  {
  const auto* func = Result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  if (func) {
    std::string funcName = func->getNameInfo().getName().getAsString();
    llvm::errs() << "Function: " << funcName << "\n";
  }
  const clang::VarDecl *vd = Result.Nodes.getNodeAs<clang::VarDecl>("VarDecl_in_main");
  if (vd) {
    llvm::errs() << vd->getType().getAsString() << "\n";
    llvm::errs() << vd->getNameAsString() << "\n";
  }
}

// bool ASTConverterClassVisitor::VisitMemberExpr(clang::MemberExpr *me) {
//   auto func_name = me->getMemberNameInfo().getAsString();
//   if(isBuiltinFunc(func_name)) {
//     llvm::errs() << "Builtin:";
//   }
//   llvm::errs() << me->getMemberNameInfo().getAsString() << "\n"; 
  
//   return true;
// }