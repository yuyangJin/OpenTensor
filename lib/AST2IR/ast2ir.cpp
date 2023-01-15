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
  if (astContext->getSourceManager().isInMainFile(
          vd->getLocation())) { // checks if the node is in the main = input
                                // file.
    std::string vd_type = vd->getType().getAsString();
    // dbg(vd_type);
    if (vd_type.compare("class Tensor") == 0) {

      std::string vd_name = vd->getQualifiedNameAsString();

      std::string malloc_str = std::string("malloc");
      std::string tensor_str = vd->getNameAsString();

      ArgList malloc_args;

      DataShape tensor_shape;

      auto malloc_node_id = _graph->addNode(
          std::make_shared<CallIRNode>(malloc_str, malloc_args));
      // auto malloc_node_id =
      // _graph->addNode(std::make_unique<CallIRNode>(malloc_str, malloc_args));
      // auto* malloc_node = new CallIRNode(malloc_str, malloc_args);
      // auto malloc_node_id = _graph->addNode(malloc_node);

      auto tensor_node_id = _graph->addNode(
          std::make_shared<DataIRNode>(tensor_str, tensor_shape));
      // auto tensor_node_id =
      // _graph->addNode(std::make_unique<DataIRNode>(tensor_str,
      // tensor_shape)); auto* tensor_node = new DataIRNode(tensor_str,
      // tensor_shape); auto tensor_node_id = _graph->addNode(tensor_node);
      _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;

      _graph->addEdge(malloc_node_id, tensor_node_id);
    }
  }
  return true;
}

enum ep_type_t {
  TENSOR = 100,
  DIM,

  // CAST,

  // Binary Op
  // ADD,
  // MINUS,
  // MUL,
  // DIV,

  BIN

};

class ExprPat { // Node
private:
  ep_type_t _type;
  std::string _name;

  std::vector<ExprPat *> _children;
  int _numChildren;

public:
  ExprPat(ep_type_t type, std::string name) : _type(type), _name(name) {
    _numChildren = 0;
  }
  ExprPat(ep_type_t type, char *name) : _type(type), _name(std::string(name)) {
    _numChildren = 0;
  }

  ep_type_t getType() { return _type; }
  std::string &getName() { return _name; }
  int getNumChildren() { return _numChildren; }
  void addChild(ExprPat *node) {
    _children.push_back(node);
    _numChildren++;
  }

  void getTensors(std::vector<std::string> &tensor_list) {
    if (getType() == TENSOR) {
      tensor_list.push_back(getName());
      return;
    }
    for (auto *child : _children) {
      child->getTensors(tensor_list);
    }
  }

  void getDims(std::vector<std::string> &dim_list) {
    if (getType() == DIM) {
      dim_list.push_back(getName());
      return;
    }
    for (auto *child : _children) {
      child->getDims(dim_list);
    }
  }

  std::string toString() {
    std::string str("");
    if (getType() == TENSOR) { // directly
      str += std::string(getName());
      for (auto *child : _children) {
        if (child->getType() == DIM) {
          str += '[' + child->getName() + ']';
        }
      }
    } else if (getType() == BIN) { // binary op only has two child
      if (getNumChildren() == 2) {
        auto *lhs = _children[0];
        if (lhs->getType() == TENSOR) {
          str += lhs->toString();
        } else if (lhs->getType() == BIN) {
          str += '(' + lhs->toString() + ')';
        }

        str += getName();

        auto *rhs = _children[1];
        if (rhs->getType() == TENSOR) {
          str += rhs->toString();
        } else if (rhs->getType() == BIN) {
          str += '(' + rhs->toString() + ')';
        }
      }
    }
    return str;
  }
};

ExprPat *
getEP(const clang::CXXOperatorCallExpr *cxxoce) { // for CXXOperatorCallExpr

  ExprPat *node;

  auto op = cxxoce->getOperator();
  if (op == clang::OO_Subscript) {
    node = nullptr;
  } else if (op == clang::OO_Star) {
    node = new ExprPat(BIN, "*");
  }

  for (auto *child : cxxoce->children()) {
    ExprPat *child_node = nullptr;
    if (clang::isa<clang::ImplicitCastExpr>(child)) {
      // donothing
    } else if (clang::isa<clang::DeclRefExpr>(child)) {
      const auto *dre_child = clang::dyn_cast<clang::DeclRefExpr>(child);
      std::string dre_child_type = dre_child->getType().getAsString();
      std::string dre_child_name = dre_child->getNameInfo().getAsString();
      if (dre_child_type.compare("class Tensor") == 0) {
        child_node = new ExprPat(TENSOR, dre_child_name);
      } else if (dre_child_type.compare("class Dim") == 0) {
        child_node = new ExprPat(DIM, dre_child_name);
      }

    } else if (clang::isa<clang::CXXOperatorCallExpr>(child)) {
      const auto *cxxoce_child =
          clang::dyn_cast<clang::CXXOperatorCallExpr>(child);
      child_node = getEP(cxxoce_child);
    }

    if (child_node != nullptr) {
      if (node == nullptr) { // if child is left child of '[]' operator, then
                             // take it as parent node
        node = child_node;
      } else {
        node->addChild(child_node);
      }
    }
  }

  return node;
}

bool ASTConverterClassVisitor::VisitCXXMemberCallExpr(
    clang::CXXMemberCallExpr *cmce) {
  ArgList call_args;

  for (const clang::Stmt *child : cmce->children()) {
    if (clang::isa<clang::MemberExpr>(child)) {
      const auto *me = clang::dyn_cast<clang::MemberExpr>(child);
      std::string callee_str = me->getMemberNameInfo().getAsString();
      if (callee_str.compare("einsum") == 0) {
        // get LHS & write memnode
        ExprPat* lhs_ep;
        ExprPat* rhs_ep;
        std::string lhs_str;
        std::string rhs_str;
        ReductionMode reduction_mode;
        std::vector<std::string> write_tensors;
        std::vector<std::string> read_tensors;

        for (const auto *me_child : me->children()) {
          if (clang::isa<clang::CXXOperatorCallExpr>(me_child)) {
            const auto *cxxoce =
                clang::dyn_cast<clang::CXXOperatorCallExpr>(me_child);
            lhs_ep = getEP(cxxoce);
            lhs_str = lhs_ep->toString();
            dbg(lhs_str);

            lhs_ep->getTensors(write_tensors);
          }
        }

        for (const auto *einsum_child : cmce->children()) {
          if (clang::isa<clang::CXXMemberCallExpr>(
                  einsum_child)) { // reduction function
            const auto *reduction_cxxmce =
                clang::dyn_cast<clang::CXXMemberCallExpr>(einsum_child);
            std::string reduction_type_str;

            for (const auto *reduction_cxxmce_child :
                 reduction_cxxmce->children()) {
              // get reduction mode
              if (const auto *reduction_me = clang::dyn_cast<clang::MemberExpr>(
                      reduction_cxxmce_child)) {
                reduction_type_str =
                    reduction_me->getMemberNameInfo().getAsString();
                dbg(reduction_type_str);
                if (reduction_type_str.compare("sum") == 0) {
                  reduction_mode.setType(SUM);
                }

                // get RHS & read memnode
                for (const auto *reduction_me_child :
                     reduction_me->children()) {
                  if (const auto *rhs_pe = clang::dyn_cast<clang::ParenExpr>(
                          reduction_me_child)) {

                    for (const auto *rhs_pe_child : rhs_pe->children()) {
                      if (const auto *rhs_cxxoce =
                              clang::dyn_cast<clang::CXXOperatorCallExpr>(
                                  rhs_pe_child)) {

                        rhs_ep = getEP(rhs_cxxoce);
                        rhs_str = rhs_ep->toString();
                        dbg(rhs_ep->toString());

                        rhs_ep->getTensors(read_tensors);
                      }
                    }
                  }
                }

              } else if (const auto *reduction_dre =
                             clang::dyn_cast<clang::DeclRefExpr>(
                                 reduction_cxxmce_child)) {
                // reduction pattern
                reduction_mode.addDim(
                    reduction_dre->getNameInfo().getAsString());
              }
            }
          }
        }

        // build nodes
        // build parallel node
        DataShape para_shape;
        lhs_ep->getDims(para_shape._dims);
        dbg(para_shape._dims);
        auto para_node = std::make_shared<ParaIRNode>(para_shape);
        auto para_node_id =
            _graph->addNode(para_node);

        // build einsum task node
        auto einsum_node_id =
            _graph->addNode(std::make_shared<EinsumTaskIRNode>(lhs_str, rhs_str,
                                                               reduction_mode));
        para_node->addBodyNode(einsum_node_id);


        // build memory access node
        for (auto &tensor : write_tensors) {
          auto mem_node_id =
              _graph->addNode(std::make_shared<MemIRNode>(tensor, WRITE, DRAM));
          para_node->addBodyNode(mem_node_id);
          auto tensor_node_id = _tensor_name_2_irnode_id[tensor];
          _graph->addEdge(tensor_node_id, mem_node_id);


          DataShape tensor_shape;
          auto new_tensor_node_id = _graph->addNode(
              std::make_shared<DataIRNode>(tensor, tensor_shape));
          _tensor_name_2_irnode_id[tensor] = new_tensor_node_id;
          _graph->addEdge(mem_node_id, new_tensor_node_id);
          _graph->addEdge(einsum_node_id, mem_node_id);
        }

        for (auto &tensor : read_tensors) {
          auto mem_node_id =
              _graph->addNode(std::make_shared<MemIRNode>(tensor, READ, DRAM));
          para_node->addBodyNode(mem_node_id);

          _graph->addEdge(mem_node_id, einsum_node_id);

          auto tensor_node_id = _tensor_name_2_irnode_id[tensor];
          _graph->addEdge(tensor_node_id, mem_node_id);
        }

        

      } else {

        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            std::string dre_type = dre->getType().getAsString();
            dbg(dre_type);
            if (dre_type.compare("class Tensor") == 0) {

              auto call_node_id = _graph->addNode(
                  std::make_shared<CallIRNode>(callee_str, call_args));
              // auto call_node_id =
              // _graph->addNode(std::make_unique<CallIRNode>(callee_str,
              // call_args)); auto* call_node = new CallIRNode(callee_str,
              // call_args); auto call_node_id = _graph->addNode(call_node);

              llvm::errs() << dre->getNameInfo().getAsString() << "\n";
              std::string tensor_str = dre->getNameInfo().getAsString();
              auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
              _graph->addEdge(data_node_id, call_node_id);

              if(callee_str.compare("print") != 0) {
                DataShape tensor_shape;
                auto tensor_node_id = _graph->addNode(
                    std::make_shared<DataIRNode>(tensor_str, tensor_shape));
                // auto tensor_node_id =
                // _graph->addNode(std::make_unique<DataIRNode>(tensor_str,
                // tensor_shape)); auto* tensor_node = new DataIRNode(tensor_str,
                // tensor_shape); auto tensor_node_id =
                // _graph->addNode(tensor_node);
                _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;
                _graph->addEdge(call_node_id, tensor_node_id);
              }
            }
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

    auto malloc_node_id =
        _graph->addNode(std::make_shared<CallIRNode>(malloc_str, malloc_args));
    // auto malloc_node_id =
    // _graph->addNode(std::make_unique<CallIRNode>(malloc_str, malloc_args));
    // auto* malloc_node = new CallIRNode(malloc_str, malloc_args);
    // auto malloc_node_id = _graph->addNode(malloc_node);

    auto tensor_node_id =
        _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));
    // auto tensor_node_id =
    // _graph->addNode(std::make_unique<DataIRNode>(tensor_str, tensor_shape));
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
        auto call_node_id = _graph->addNode(
            std::make_shared<CallIRNode>(callee_str, call_args));
        // auto call_node_id =
        // _graph->addNode(std::make_unique<CallIRNode>(callee_str, call_args));
        // auto* call_node = new CallIRNode(callee_str, call_args);
        // auto call_node_id = _graph->addNode(call_node);
        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            llvm::errs() << dre->getNameInfo().getAsString() << "\n";
            std::string tensor_str = dre->getNameInfo().getAsString();
            auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
            _graph->addEdge(data_node_id, call_node_id);

            DataShape tensor_shape;
            auto tensor_node_id = _graph->addNode(
                std::make_shared<DataIRNode>(tensor_str, tensor_shape));
            // auto tensor_node_id =
            // _graph->addNode(std::make_unique<DataIRNode>(tensor_str,
            // tensor_shape)); auto* tensor_node = new DataIRNode(tensor_str,
            // tensor_shape); auto tensor_node_id =
            // _graph->addNode(tensor_node);
            _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;
            _graph->addEdge(call_node_id, tensor_node_id);
          }
        }
      }
    }
  }
}
