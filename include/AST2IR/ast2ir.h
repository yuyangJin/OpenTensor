#ifndef AST_TO_IR_H_
#define AST_TO_IR_H_

#include "IR/irgraph.h"

// Clang includes
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/AST/RecursiveASTVisitor.h"
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

// class ASTConverterClassVisitor
//     : public clang::RecursiveASTVisitor<ASTConverterClassVisitor> {
// private:
//   clang::ASTContext *astContext; // used for getting additional AST info
//   std::vector<clang::ArraySubscriptExpr *> visitedASE;
//   int recursiveCount = 0;

// public:
//   explicit ASTConverterClassVisitor(clang::CompilerInstance *CI)
//       : astContext(&(CI->getASTContext())) {}

//   virtual bool VisitFunctionDecl(clang::FunctionDecl *func) {
//     // std::string funcName = func->getNameInfo().getName().getAsString();
//     // llvm::errs() << "Function: " << funcName << "\n";
//     return true;
//   }

//   virtual bool VisitArraySubscriptExpr(clang::ArraySubscriptExpr *array) {
//     return true;
//   }

//   virtual bool VisitStmt(clang::Stmt *st) {
//     if (auto *ret = llvm::dyn_cast<clang::ReturnStmt>(st)) {
//     }
//     if (auto *call = llvm::dyn_cast<clang::CallExpr>(st)) {
//     }
//     return true;
//   }

//   // virtual bool VisitDeclStmt(clang::DeclStmt *decl) {
//   //   clang::Stmt::child_range children = decl->children();
//   //   for (clang::Stmt* child: children) {
//   //     if (llvm::isa<clang::VarDecl>(child)) {
//   //       clang::VarDecl *var_decl = (clang::VarDecl*)child;
//   //       var_decl->getType().dump();
//   //     }
//   //   }
//   //   return true;
//   // }

//   // virtual bool VisitVarDecl(clang::VarDecl *vd) ;

//   virtual bool VisitMemberExpr(clang::MemberExpr *me) ;
// };


class ASTConverterCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  ASTConverterCallback(){}
  void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override {
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
};


class ASTConverterClassConsumer : public clang::ASTConsumer {
private:
  // ASTConverterClassVisitor *visitor; // doesn't have to be private
  clang::ast_matchers::MatchFinder match_finder; 
  ASTConverterCallback handler; 

public:
  // override the constructor in order to pass CI
  explicit ASTConverterClassConsumer(clang::CompilerInstance *CI) {
    match_finder.addMatcher(clang::ast_matchers::varDecl(clang::ast_matchers::isExpansionInMainFile()).bind("VarDecl_in_main"), &handler);
    const auto matching_node = clang::ast_matchers::functionDecl(clang::ast_matchers::isExpansionInMainFile()).bind("fn");
        match_finder.addMatcher(matching_node, &handler);
    // visitor = new ASTConverterClassVisitor(CI);
    // match_finder.addMatcher(clang::ast_matchers::varDecl(clang::ast_matchers::hasAncestor(clang::ast_matchers::functionDecl())).bind("VarDecl_hasAncestor_functionDecl"), &handler);
  }

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    // visitor->TraverseDecl(Context.getTranslationUnitDecl());
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
