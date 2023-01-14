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

bool ASTConverterClassVisitor::VisitVarDecl(clang::VarDecl *vd) {
  if (astContext->getSourceManager().isInMainFile(vd->getLocation())) { //checks if the node is in the main = input file.
    std::string vd_type = vd->getType().getAsString();
    // dbg(vd_type);
    if (vd_type.compare("class Tensor") == 0) {
    
      std::string vd_name = vd->getQualifiedNameAsString();
    
      std::string malloc_str = std::string("malloc");
      std::string tensor_str = vd->getNameAsString();
      
      ArgList malloc_args;

      DataShape tensor_shape;

      auto malloc_node_id = _graph->addNode(std::make_shared<CallIRNode>(malloc_str, malloc_args));
      // auto malloc_node_id = _graph->addNode(std::make_unique<CallIRNode>(malloc_str, malloc_args));
      // auto* malloc_node = new CallIRNode(malloc_str, malloc_args);
      // auto malloc_node_id = _graph->addNode(malloc_node);



      auto tensor_node_id = _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));
      // auto tensor_node_id = _graph->addNode(std::make_unique<DataIRNode>(tensor_str, tensor_shape));
      // auto* tensor_node = new DataIRNode(tensor_str, tensor_shape);
      // auto tensor_node_id = _graph->addNode(tensor_node);
      _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;


      _graph->addEdge(malloc_node_id, tensor_node_id);  

    }
            
  }
  return true;
}

bool ASTConverterClassVisitor::VisitCXXMemberCallExpr(clang::CXXMemberCallExpr *cmce) {
  ArgList call_args;

  for (const clang::Stmt *child : cmce->children()) {
    if (clang::isa<clang::MemberExpr>(child)) {
      const auto *me = clang::dyn_cast<clang::MemberExpr>(child);

        // llvm::errs() << me->getMemberNameInfo().getAsString() << "\n";

        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            std::string dre_type = dre->getType().getAsString();
            dbg(dre_type);
            if (dre_type.compare("class Tensor") == 0) {



              std::string callee_str = me->getMemberNameInfo().getAsString();
              auto call_node_id = _graph->addNode(std::make_shared<CallIRNode>(callee_str, call_args));
              // auto call_node_id = _graph->addNode(std::make_unique<CallIRNode>(callee_str, call_args));
              // auto* call_node = new CallIRNode(callee_str, call_args);
              // auto call_node_id = _graph->addNode(call_node);

              llvm::errs() << dre->getNameInfo().getAsString() << "\n";
              std::string tensor_str = dre->getNameInfo().getAsString();
              auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
              _graph->addEdge(data_node_id, call_node_id);

              DataShape tensor_shape;
              auto tensor_node_id = _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));
              // auto tensor_node_id = _graph->addNode(std::make_unique<DataIRNode>(tensor_str, tensor_shape));
              // auto* tensor_node = new DataIRNode(tensor_str, tensor_shape);
              // auto tensor_node_id = _graph->addNode(tensor_node);
              _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;
              _graph->addEdge(call_node_id, tensor_node_id);
            }
          }
        }
      
    }
  }
  return true;
}


ASTConverterClassConsumer::ASTConverterClassConsumer(
    clang::CompilerInstance *CI) {
  // match_finder.addMatcher(
  //     clang::ast_matchers::varDecl(
  //         clang::ast_matchers::isExpansionInMainFile(),
  //         clang::ast_matchers::hasType(
  //             clang::ast_matchers::asString("class Tensor")))
  //         .bind("TensorVarDecl"),
  //     &handler);
  // match_finder.addMatcher(clang::ast_matchers::functionDecl(
  //                             clang::ast_matchers::isExpansionInMainFile())
  //                             .bind("FuncDecl_In_MainFile"),
  //                         &handler);
  // match_finder.addMatcher(
  //     clang::ast_matchers::cxxMemberCallExpr(
  //         clang::ast_matchers::isExpansionInMainFile(),
  //         clang::ast_matchers::on(
  //             clang::ast_matchers::hasType(clang::ast_matchers::cxxRecordDecl(
  //                 clang::ast_matchers::hasName("Tensor")))))
  //         .bind("TensorCXXMemberCallExpr"),
  //     &handler);
  visitor = new ASTConverterClassVisitor(CI);
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

    std::string malloc_str = std::string("malloc");
    std::string tensor_str = vd->getNameAsString();
    
    ArgList malloc_args;

    DataShape tensor_shape;

    // for (const auto child: vd->children()) {
    //   if (clang::isa<clang::CXXConstructExpr>(child)) {

    //   }
    // }
    // for vd->get


    auto malloc_node_id = _graph->addNode(std::make_shared<CallIRNode>(malloc_str, malloc_args));
    // auto malloc_node_id = _graph->addNode(std::make_unique<CallIRNode>(malloc_str, malloc_args));
    // auto* malloc_node = new CallIRNode(malloc_str, malloc_args);
    // auto malloc_node_id = _graph->addNode(malloc_node);



    auto tensor_node_id = _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));
    // auto tensor_node_id = _graph->addNode(std::make_unique<DataIRNode>(tensor_str, tensor_shape));
    // auto* tensor_node = new DataIRNode(tensor_str, tensor_shape);
    // auto tensor_node_id = _graph->addNode(tensor_node);
    _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;


    _graph->addEdge(malloc_node_id, tensor_node_id);  
  }

  if (const auto *cmce = Result.Nodes.getNodeAs<clang::CXXMemberCallExpr>(
          "TensorCXXMemberCallExpr")) {
    llvm::errs() << "TensorCXXMemberCallExpr:"
                 << "\n";
    ArgList call_args;

    for (const clang::Stmt *child : cmce->children()) {
      if (clang::isa<clang::MemberExpr>(child)) {
        const auto *me = clang::dyn_cast<clang::MemberExpr>(child);
        llvm::errs() << me->getMemberNameInfo().getAsString() << "\n";
        std::string callee_str = me->getMemberNameInfo().getAsString();
        auto call_node_id = _graph->addNode(std::make_shared<CallIRNode>(callee_str, call_args));
        // auto call_node_id = _graph->addNode(std::make_unique<CallIRNode>(callee_str, call_args));
        // auto* call_node = new CallIRNode(callee_str, call_args);
        // auto call_node_id = _graph->addNode(call_node);
        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            llvm::errs() << dre->getNameInfo().getAsString() << "\n";
            std::string tensor_str = dre->getNameInfo().getAsString();
            auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
            _graph->addEdge(data_node_id, call_node_id);

            DataShape tensor_shape;
            auto tensor_node_id = _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));
            // auto tensor_node_id = _graph->addNode(std::make_unique<DataIRNode>(tensor_str, tensor_shape));
            // auto* tensor_node = new DataIRNode(tensor_str, tensor_shape);
            // auto tensor_node_id = _graph->addNode(tensor_node);
            _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;
            _graph->addEdge(call_node_id, tensor_node_id);
          }
        }
      }
    }
    
    


  }
}
