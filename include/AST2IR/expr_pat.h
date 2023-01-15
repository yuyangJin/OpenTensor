#ifndef EXPR_PAT_H_
#define EXPR_PAT_H_

#include <string>
#include <vector>

enum ep_type_t {
  TENSOR = 100,
  DIM,
  BIN,
};

/**
 * @brief Expression pattern tree
 *
 */
class ExprPat {
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
  void addChild(ExprPat *node);

  void getTensors(std::vector<std::string> &tensor_list);

  void getDims(std::vector<std::string> &dim_list);

  std::string toString();
};

#endif // EXPR_PAT_H_