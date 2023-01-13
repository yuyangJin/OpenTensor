#ifndef AST_TO_IR_H_
#define AST_TO_IR_H_

#include "IR/irgraph.h"

// Clang includes
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
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


class ASTConverterCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  ASTConverterCallback(){}
  void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};


class ASTConverterClassConsumer : public clang::ASTConsumer {
private:
  clang::ast_matchers::MatchFinder match_finder; 
  ASTConverterCallback handler; 

public:
  // override the constructor in order to pass CI
  explicit ASTConverterClassConsumer(clang::CompilerInstance *CI);

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    match_finder.matchAST(Context);
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
