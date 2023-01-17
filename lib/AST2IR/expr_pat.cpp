#include "AST2IR/expr_pat.h"
#include "dbg.h"
void ExprPat::addChild(ExprPat *node) {
  _children.push_back(node);
  _numChildren++;
}

void ExprPat::getTensors(std::vector<std::string> &tensor_list) {
  if (getType() == TENSOR) {
    tensor_list.push_back(getName());
    return;
  }
  for (auto *child : _children) {
    child->getTensors(tensor_list);
  }
}

void ExprPat::getDims(std::vector<std::string> &dim_list) {
  if (getType() == DIM) {
    dim_list.push_back(getName());
    return;
  }
  for (auto *child : _children) {
    child->getDims(dim_list);
  }
}

ExprPat *ExprPat::getTensorByName(std::string &tensor_name) {
  if (getType() == TENSOR && tensor_name.compare(getName()) == 0) {
    return this;
  }
  for (auto *child : _children) {
    return child->getTensorByName(tensor_name);
  }
  return nullptr;
}

std::vector<std::string> *ExprPat::getDimsOfTensor(std::string &tensor_name) {
  auto *dim_list = new std::vector<std::string>();

  auto *tensor_node = getTensorByName(tensor_name);

  dbg(tensor_node);

  if (tensor_node != nullptr) {
    for (auto *child : tensor_node->getChildren()) {
      if (child->getType() == DIM) {
        dim_list->emplace_back(child->getName());
      }
    }
  }
  return dim_list;
}

std::string ExprPat::toString() {
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