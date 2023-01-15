#include "AST2IR/ast2ir.h"
#include "AST2IR/expr_pat.h"

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

/**
 * @brief Identify tensor data declaration
 *
 * @param vd - clang::VarDecl
 * @return true
 * @return false
 */
bool ASTConverterClassVisitor::VisitVarDecl(clang::VarDecl *vd) {
  if (astContext->getSourceManager().isInMainFile(
          vd->getLocation())) { // Check whether the node is in the main input
                                // file.
    std::string vd_type = vd->getType().getAsString();

    if (vd_type.compare("class Tensor") ==
        0) { // Check whether the node is Tensor declaration

      std::string malloc_str = std::string("malloc");
      std::string tensor_str = vd->getNameAsString(); // Tensor name

      // Get Tensor shape
      DataShape tensor_shape;
      ArgList malloc_args;
      /** TODO: Get Tenosr shape
       */

      // Build malloc node and tensor data node
      auto malloc_node_id = _graph->addNode(
          std::make_shared<CallIRNode>(malloc_str, malloc_args));

      auto tensor_node_id = _graph->addNode(
          std::make_shared<DataIRNode>(tensor_str, tensor_shape));
      // Record latest tensor data node of correspond tensor
      _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;

      // Build edges between malloc node and tensor data node
      _graph->addEdge(malloc_node_id, tensor_node_id);
    }
  }
  return true;
}

/**
 * @brief Extract expression pattern tree from clang AST. Recursively convert
 * clang AST format to designed ExprPat(Tree) for further analysis. Below shows
 * an example.
 *
 *  Example code: A[i][k] * B[k][j]
 *
 *            clang AST:                  ExprPat(Tree)
 *
 *                '*'                          '*'
 *               /   \                        /   \
 *             '[]'  '[]'        getEP        A    B
 *            / \    / \         ==>        / \  / \
 *         '[]' k  '[]' j                   i k  k j
 *         / \     / \
 *         A i     B k
 *
 * @param cxxoce - const CXXOperatorCallExpr*
 * @return ExprPat*
 */
ExprPat *getEP(const clang::CXXOperatorCallExpr *cxxoce) {

  ExprPat *node; // Root node of the CXXOperatorCallExpr

  auto op = cxxoce->getOperator();
  if (op == clang::OO_Subscript) { // For operator '[]', mark the root
                                   // ExprPatTree node as nullptr, so that the
                                   // left (first) AST child of operator '[]'
                                   // can be set as root ExprPatTree node.
    node = nullptr;
  } else if (op == clang::OO_Star) { // For other binary operators, set binary
                                     // op as their root node.
    node = new ExprPat(BIN, "*");
  }

  for (auto *child : cxxoce->children()) {
    ExprPat *child_node = nullptr;
    if (clang::isa<clang::ImplicitCastExpr>(child)) { // do nothing for cast op

    } else if (const auto *dre_child = clang::dyn_cast<clang::DeclRefExpr>(
                   child)) { // For DeclRefExpr
      // Build ExprPatNode for Tensor and Dim
      std::string dre_child_type = dre_child->getType().getAsString();
      std::string dre_child_name = dre_child->getNameInfo().getAsString();
      if (dre_child_type.compare("class Tensor") == 0) {
        child_node = new ExprPat(TENSOR, dre_child_name);
      } else if (dre_child_type.compare("class Dim") == 0) {
        child_node = new ExprPat(DIM, dre_child_name);
      }
    } else if (const auto *cxxoce_child =
                   clang::dyn_cast<clang::CXXOperatorCallExpr>(
                       child)) { // For CXXOperatorCallExpr
      child_node =
          getEP(cxxoce_child); // Recursively extract its ExprPat Node(Tree).
    }

    if (child_node != nullptr) { // If child is not cast op
      if (node == nullptr) { // If child is left child of '[]' operator, then
                             // take it as parent node
        node = child_node;
      } else { // For other cases
        node->addChild(child_node);
      }
    }
  }

  return node;
}

/**
 * @brief Identify Tensor function calls
 *
 * @param cmce - CXXMemberCallExpr
 * @return true
 * @return false
 */
bool ASTConverterClassVisitor::VisitCXXMemberCallExpr(
    clang::CXXMemberCallExpr *cmce) {
  /** TODO: Check if it is in main file
   */
  /** TODO: Check if it is class Tensor's member functions
   */

  ArgList call_args;

  for (const clang::Stmt *child : cmce->children()) {
    if (const auto *me = clang::dyn_cast<clang::MemberExpr>(child)) {

      // Get callee function name
      std::string callee_str = me->getMemberNameInfo().getAsString();

      if (callee_str.compare("einsum") ==
          0) { // Convert einsum notation to IRNode
        /**  Einsum notation expression is : LHS    .einsum((RHS
         * ).REDUCTION_TYPE(REDUCTION_DIMS)) C[i][j].einsum((A[i][k] *
         * B[k][j]).sum           (k             )) Here we need to get LHS,
         * RHS, REDUCTION_TYPE, and REDUCTION_DIMS from clang AST.
         */

        ExprPat *lhs_ep; // Contain LHS
        ExprPat *rhs_ep; // Contain RHS

        ReductionMode
            reduction_mode; // Contain REDUCTION_TYPE and REDUCTION_DIMS

        // Get LHS by getEP function
        for (const auto *me_child : me->children()) {
          if (const auto *lhs_cxxoce =
                  clang::dyn_cast<clang::CXXOperatorCallExpr>(me_child)) {
            lhs_ep = getEP(lhs_cxxoce);
          }
        }

        for (const auto *einsum_child : cmce->children()) {
          if (const auto *arg_cxxmce =
                  clang::dyn_cast<clang::CXXMemberCallExpr>(
                      einsum_child)) { // Handle the arg of einsum function
                                       // (between parentheses)

            for (const auto *reduction_type_cxxmce_child :
                 arg_cxxmce->children()) {

              if (const auto *reduction_type_me =
                      clang::dyn_cast<clang::MemberExpr>(
                          reduction_type_cxxmce_child)) { // Get REDUCTION TYPE
                std::string reduction_type_str =
                    reduction_type_me->getMemberNameInfo().getAsString();

                // dbg(reduction_type_str);
                if (reduction_type_str.compare("sum") == 0) {
                  reduction_mode.setType(SUM);
                } else if (reduction_type_str.compare("max") == 0) {
                  reduction_mode.setType(MAX);
                } else if (reduction_type_str.compare("min") == 0) {
                  reduction_mode.setType(MIN);
                } else if (reduction_type_str.compare("avg") == 0) {
                  reduction_mode.setType(AVG);
                }

                // Get RHS by getEP function
                for (const auto *reduction_me_child :
                     reduction_type_me->children()) {
                  if (const auto *rhs_pe = clang::dyn_cast<clang::ParenExpr>(
                          reduction_me_child)) { // The AST child of
                                                 // reduction_me is (RHS)

                    for (const auto *rhs_pe_child : rhs_pe->children()) {
                      if (const auto *rhs_cxxoce =
                              clang::dyn_cast<clang::CXXOperatorCallExpr>(
                                  rhs_pe_child)) {
                        rhs_ep = getEP(rhs_cxxoce);
                      }
                    }
                  }
                }

              } else if (const auto *reduction_dre =
                             clang::dyn_cast<clang::DeclRefExpr>(
                                 reduction_type_cxxmce_child)) { // Get
                                                                 // REDUCTION
                                                                 // DIMS
                reduction_mode.addDim(
                    reduction_dre->getNameInfo().getAsString());
              }
            }
          }
        }

        /** Build nodes */
        // Build ParaIRNode
        DataShape para_shape;
        lhs_ep->getDims(para_shape._dims);
        /** TODO: Get parallelism shape (same as shape of dims, due to tensor
         * data is regular)
         */
        // dbg(para_shape._dims);
        auto para_node = std::make_shared<ParaIRNode>(para_shape);
        auto para_node_id = _graph->addNode(para_node);

        // Build EinsumTaskIRNode
        std::string lhs_str;
        std::string rhs_str;
        lhs_str = lhs_ep->toString();
        // dbg(lhs_str);
        rhs_str = rhs_ep->toString();
        // dbg(rhs_str);
        auto einsum_node_id =
            _graph->addNode(std::make_shared<EinsumTaskIRNode>(lhs_str, rhs_str,
                                                               reduction_mode));
        para_node->addBodyNode(
            einsum_node_id); // EinsumTaskIRNode is body node of ParaIRNode

        // Build MemIRNode, including read & write node
        std::vector<std::string> write_tensors;
        std::vector<std::string> read_tensors;
        lhs_ep->getTensors(write_tensors);
        rhs_ep->getTensors(read_tensors);

        // Read MemIRNode
        for (auto &tensor : read_tensors) {
          auto mem_node_id =
              _graph->addNode(std::make_shared<MemIRNode>(tensor, READ, DRAM));
          para_node->addBodyNode(
              mem_node_id); // MemIRNode is body node of ParaIRNode
          // Build edges bewtween input tensor data node and read memory access
          // node, as well as read memory access node and einsum task node
          auto tensor_node_id = _tensor_name_2_irnode_id[tensor];
          _graph->addEdge(tensor_node_id, mem_node_id);
          _graph->addEdge(mem_node_id, einsum_node_id);
        }
        // Write MemIRNode
        for (auto &tensor : write_tensors) {
          auto mem_node_id =
              _graph->addNode(std::make_shared<MemIRNode>(tensor, WRITE, DRAM));
          para_node->addBodyNode(
              mem_node_id); // MemIRNode is body node of ParaIRNode

          // Build an edge bewtween write memory access node and einsum task
          // node
          auto tensor_node_id = _tensor_name_2_irnode_id[tensor];
          _graph->addEdge(tensor_node_id, mem_node_id);

          // Build output tensor data node for WRITE operation
          DataShape tensor_shape;
          /** TODO: Get Tensor shape
           */
          auto output_tensor_node_id = _graph->addNode(
              std::make_shared<DataIRNode>(tensor, tensor_shape));
          // Build edges bewtween einsum task node & write memory access node,
          // as well as write memory access node and output tensor data node
          _graph->addEdge(einsum_node_id, mem_node_id);
          _tensor_name_2_irnode_id[tensor] = output_tensor_node_id;
          _graph->addEdge(mem_node_id, output_tensor_node_id);
        }

      } else { // For other Tensor member (builtin) function calls

        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            std::string dre_type = dre->getType().getAsString();

            if (dre_type.compare("class Tensor") ==
                0) { // Determine whether it is member functions of class Tensor

              // Build CallIRNode for Tensor member function calls
              auto call_node_id = _graph->addNode(
                  std::make_shared<CallIRNode>(callee_str, call_args));

              std::string tensor_str = dre->getNameInfo().getAsString();
              // Build edges bewtween input tensor data node & call node
              auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
              _graph->addEdge(data_node_id, call_node_id);

              if (callee_str.compare("print") !=
                  0) { // Build output tensor data node except for print
                       // function.
                DataShape tensor_shape;
                /** TODO: Get Tensor shape
                 */
                auto tensor_node_id = _graph->addNode(
                    std::make_shared<DataIRNode>(tensor_str, tensor_shape));
                // Build edges bewtween call node & output tensor data node
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

  /** AST visitor
   */
  visitor = new ASTConverterClassVisitor(CI);

  /** Below is code for AST matcher, the biggest problem is that it does not
   * follow the AST order, but follows the order of added AST matcher rules.
   */
  /*
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
  */
}

/**
 * @brief Using AST matcher to convert AST to IRGraph
 *
 * @param Result
 */
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

    auto malloc_node_id =
        _graph->addNode(std::make_shared<CallIRNode>(malloc_str, malloc_args));

    auto tensor_node_id =
        _graph->addNode(std::make_shared<DataIRNode>(tensor_str, tensor_shape));

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

        for (const auto *subchild : me->children()) {
          if (const auto *dre = clang::dyn_cast<clang::DeclRefExpr>(subchild)) {
            llvm::errs() << dre->getNameInfo().getAsString() << "\n";
            std::string tensor_str = dre->getNameInfo().getAsString();
            auto data_node_id = _tensor_name_2_irnode_id[tensor_str];
            _graph->addEdge(data_node_id, call_node_id);

            DataShape tensor_shape;
            auto tensor_node_id = _graph->addNode(
                std::make_shared<DataIRNode>(tensor_str, tensor_shape));

            _tensor_name_2_irnode_id[tensor_str] = tensor_node_id;
            _graph->addEdge(call_node_id, tensor_node_id);
          }
        }
      }
    }
  }
}
