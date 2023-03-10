#ifndef AST_TO_IR_H_
#define AST_TO_IR_H_

#include "IR/irgraph.h"

// Clang includes
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

// Std includes
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// std::unique_ptr<IRGraph> _graph;
IRGraph *_graph;
std::unordered_map<std::string, irnode_id_t>
    _tensor_name_2_irnode_id; // latest_irnode

class ASTConverterClassVisitor
    : public clang::RecursiveASTVisitor<ASTConverterClassVisitor> {
private:
  clang::ASTContext *astContext; // used for getting additional AST info
public:
  explicit ASTConverterClassVisitor(clang::CompilerInstance *CI)
      : astContext(&(CI->getASTContext())) {}
  virtual bool VisitVarDecl(clang::VarDecl *vd);
  virtual bool VisitCXXMemberCallExpr(clang::CXXMemberCallExpr *cmce);
};

class ASTConverterCallback
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  ASTConverterCallback() {}
  void
  run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

class ASTConverterClassConsumer : public clang::ASTConsumer {
private:
  // clang::ast_matchers::MatchFinder match_finder;
  // ASTConverterCallback handler;
  ASTConverterClassVisitor *visitor;

public:
  // override the constructor in order to pass CI
  explicit ASTConverterClassConsumer(clang::CompilerInstance *CI);

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    // match_finder.matchAST(Context);
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
    // _graph = std::make_unique<IRGraph>();
    _graph = new IRGraph();

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

  IRGraph *getIRGraph() { return _graph; }
};

#endif // AST_TO_IR_H_
