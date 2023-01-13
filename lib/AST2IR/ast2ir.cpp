#include "AST2IR/ast2ir.h"

#define FUNC_LIST_SIZE 10
std::string builtin_func_list[] = {"random", "zeros", "print"};

bool isBuiltinFunc(std::string &func) {
  for (size_t i = 0; i < FUNC_LIST_SIZE; i++) {
    if (func == builtin_func_list[i]) {
      return true;
    }
  }
  return false;
}

ASTConverterClassConsumer::ASTConverterClassConsumer(
    clang::CompilerInstance *CI) {
  match_finder.addMatcher(
      clang::ast_matchers::varDecl(
          clang::ast_matchers::isExpansionInMainFile(),
          clang::ast_matchers::hasType(
              clang::ast_matchers::asString("class Tensor")))
          .bind("TensorVarDecl"),
      &handler);
  match_finder.addMatcher(clang::ast_matchers::functionDecl(
                              clang::ast_matchers::isExpansionInMainFile())
                              .bind("FuncDecl_In_MainFile"),
                          &handler);
  match_finder.addMatcher(
      clang::ast_matchers::cxxMemberCallExpr(
          clang::ast_matchers::isExpansionInMainFile(),
          clang::ast_matchers::on(
              clang::ast_matchers::hasType(clang::ast_matchers::cxxRecordDecl(
                  clang::ast_matchers::hasName("Tensor")))))
          .bind("TensorCXXMemberCallExpr"),
      &handler);
}

void ASTConverterCallback::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  if (const auto *func =
          Result.Nodes.getNodeAs<clang::FunctionDecl>("FuncDecl_In_MainFile")) {
    std::string funcName = func->getNameInfo().getName().getAsString();
    llvm::errs() << "Function: " << funcName << "\n";
  }
  if (const auto *vd =
          Result.Nodes.getNodeAs<clang::VarDecl>("TensorVarDecl")) {
    llvm::errs() << "TensorVarDecl:"
                 << "\n";
    llvm::errs() << vd->getType().getAsString() << "\n";
    llvm::errs() << vd->getNameAsString() << "\n";
  }
  if (const auto *cmce = Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>(
          "TensorCXXMemberCallExpr")) {
    llvm::errs() << "TensorCXXMemberCallExpr:"
                 << "\n";
    for (const clang::Stmt *child : cmce->children()) {
      if (clang::isa<clang::MemberExpr>(child)) {
        const auto *me = clang::dyn_cast<clang::MemberExpr>(child);
        llvm::errs() << me->getMemberNameInfo().getAsString() << "\n";
        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            llvm::errs() << dre->getNameInfo().getAsString() << "\n";
          }
        }
      }
    }
  }
}
