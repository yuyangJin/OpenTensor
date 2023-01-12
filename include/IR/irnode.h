#ifndef IRNODE_H_
#define IRNODE_H_

#include <memory>
#include <string>
#include <vector>

struct DataShape {
  std::vector<int64_t> shape;
};

struct ArgList {
  std::vector<std::string> arg_list;
};

enum reduction_type_t {
  SUM,
  MAX,
  MIN,
  AVG,
  NONE,
};

struct ReductionMode {
  std::vector<std::string> _reduction_dims;
  reduction_type_t _type;

  size_t getNumReductionDims() { return _reduction_dims.size(); }
  std::string &getReductionDim(size_t i) {
    if (i < _reduction_dims.size())
      return _reduction_dims[i];
  }
  reduction_type_t getReductionType() { return _type; }
};

class IRNode {
public:
  enum irnode_type_t {
    IRNode_DataDecl,
    IRNode_DataRef,
    IRNode_Call,
    IRNode_Mem,
    IRNode_Task,
    IRNode_EinsumTask,
    IRNode_Comm,
    IRNode_Region,
    IRNode_Parallel,
    IRNode_For,
    IRNode_Branch,
  };

  IRNode(irnode_type_t type) : _type(type) {}
  virtual ~IRNode() = default;

  irnode_type_t getType() const { return _type; }

private:
  irnode_type_t _type;
};

using IRNodeList = std::vector<std::unique_ptr<IRNode>>;

class DataDeclIRNode : public IRNode {
  // private:
  std::string _name;
  DataShape _shape;

public:
  DataDeclIRNode(std::string &name, DataShape shape)
      : IRNode(IRNode_DataDecl), _name(std::string(name)),
        _shape(std::move(shape)) {}

  std::string &getName() { return _name; }
  // ExprAST *getInitVal() { return initVal.get(); }
  const DataShape &getType() { return _shape; }
};

class DataRefIRNode : public IRNode {
private:
  std::string _name;
  DataShape _shape;

public:
};

class CallIRNode : public IRNode {
  std::string _callee_func_name;
  ArgList _args;

public:
  CallIRNode(std::string &func_name, ArgList args)
      : IRNode(IRNode_Call), _callee_func_name(std::string(func_name)),
        _args(std::move(args)) {}

  std::string &getCalleeFuncName() { return _callee_func_name; }
  ArgList &getArgs() { return _args; }
};

class MemIRNode : public IRNode {
public:
  enum mem_access_mode_t {
    READ,
    WRITE,
    READ_WRITE,
  };
  enum mem_access_type_t {
    DRAM,
  };

  MemIRNode(std::string &obj, mem_access_mode_t mode, mem_access_type_t type)
      : IRNode(IRNode_Mem), _obj(std::string(obj)), _mode(mode), _type(type) {}

  mem_access_type_t getMemAccessType() { return _type; }
  void setAccessMode(mem_access_mode_t mode) { _mode = mode; }

private:
  std::string _obj;
  mem_access_mode_t _mode;
  mem_access_type_t _type;
};

class TaskIRNode : public IRNode {

  TaskIRNode() : IRNode(IRNode_Task) {}
};

class EinsumTaskIRNode : public IRNode {
public:
  EinsumTaskIRNode(std::string &lhs, std::string &rhs,
                   ReductionMode reduction_mode)
      : IRNode(IRNode_EinsumTask), _lhs(std::string(lhs)),
        _rhs(std::string(rhs)), _reduction_mode(std::move(reduction_mode)) {}

  std::string &getLHS() { return _lhs; }
  std::string &getRHS() { return _rhs; }
  ReductionMode &getReductionMode() { return _reduction_mode; }

private:
  std::string _lhs;
  std::string _rhs;
  ReductionMode _reduction_mode;
};

class CommIRNode : public IRNode {};

class RegionIRNode : public IRNode {
  std::unique_ptr<IRNodeList> _body;

public:
  RegionIRNode(std::unique_ptr<IRNodeList> body)
      : IRNode(IRNode_Region), _body(std::move(body)) {}

  IRNodeList *getBody() { return _body.get(); }
};

class ParaIRNode : public IRNode {
  std::unique_ptr<IRNodeList> _body;
  DataShape _para_shape;

public:
  ParaIRNode(std::unique_ptr<IRNodeList> body, DataShape para_shape)
      : IRNode(IRNode_Parallel), _body(std::move(body)),
        _para_shape(std::move(para_shape)) {}

  IRNodeList *getBody() { return _body.get(); }
  DataShape &getParaShape() { return _para_shape; }
};

class CtrlIRNode : public RegionIRNode {};

class ForIRNode : public CtrlIRNode {};

class BranchIRNode : public CtrlIRNode {};

#endif // IRNODE_H_