#ifndef AST_TO_IR_H_
#define AST_TO_IR_H_

#include "IR/irgraph.h"

// Clang includes
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

// Std includes
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

std::unique_ptr<IRGraph> _graph;

class ASTConverterClassVisitor
    : public clang::RecursiveASTVisitor<ASTConverterClassVisitor> {
private:
  clang::ASTContext *astContext; // used for getting additional AST info
  std::vector<clang::ArraySubscriptExpr *> visitedASE;
  int recursiveCount = 0;

public:
  explicit ASTConverterClassVisitor(clang::CompilerInstance *CI)
      : astContext(&(CI->getASTContext())) {}

  virtual bool VisitFunctionDecl(clang::FunctionDecl *func) {
    std::string funcName = func->getNameInfo().getName().getAsString();
    llvm::errs() << "Function: " << funcName << "\n";
    return true;
  }

  virtual bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr *array) {
    return true;
  }

  virtual bool VisitStmt(clang::Stmt *st) {
    if (auto *ret = llvm::dyn_cast<clang::ReturnStmt>(st)) {
    }
    if (auto *call = llvm::dyn_cast<clang::CallExpr>(st)) {
    }
    return true;
  }

  virtual bool VisitReturnStmt(clang::ReturnStmt *ret) { return true; }

  virtual bool VisitCallExpr(clang::CallExpr *call) { return true; }
};

class ASTConverterClassConsumer : public clang::ASTConsumer {
private:
  ASTConverterClassVisitor *visitor; // doesn't have to be private

public:
  // override the constructor in order to pass CI
  explicit ASTConverterClassConsumer(clang::CompilerInstance *CI) {
    visitor = new ASTConverterClassVisitor(CI);
  }

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    visitor->TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class ASTConverterClassAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) {
    return std::unique_ptr<clang::ASTConsumer>(
        new ASTConverterClassConsumer(&CI)); // pass CI pointer to ASTConsumer
  }
};

static llvm::cl::OptionCategory MyToolCategory("opentensor-ast2ir options");

class ASTConverter {
public:
  ASTConverter(int argc, const char **argv) {

    // build IRGraph
    _graph = std::make_unique<IRGraph>();

    auto ExpectedParser =
        clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
    if (!ExpectedParser) {
      // Fail gracefully for unsupported options.
      llvm::errs() << ExpectedParser.takeError();
      return;
    }
    clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                   OptionsParser.getSourcePathList());
    Tool.run(clang::tooling::newFrontendActionFactory<ASTConverterClassAction>()
                 .get());
  }

  IRGraph *getIRGraph() { return _graph.get(); }
};

#endif // AST_TO_IR_H_
