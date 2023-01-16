#ifndef IRNODE_H_
#define IRNODE_H_

#include <memory>
#include <string>
#include <vector>

using irnode_id_t = size_t;

static int global_n_id = 0;

struct DataShape {
  std::vector<int64_t> _shape;

  size_t getNumDims() { return _shape.size(); }
  int64_t getDim(size_t i) { return _shape[i]; }
};

struct ParaShape {
  std::vector<std::string> _dims;
  std::vector<int64_t> _shape;
};

struct ArgList {
  std::vector<std::string> arg_list;
};

enum reduction_type_t {
  SUM = 50,
  MAX,
  MIN,
  AVG,
  NONE,
};

enum irnode_type_t {
  IRNode_Data,
  // IRNode_DataDecl,
  // IRNode_DataRef,
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

enum mem_access_mode_t {
  READ,
  WRITE,
  READ_WRITE,
};
enum mem_access_type_t {
  DRAM,
};

struct ReductionMode {
  std::vector<std::string> _reduction_dims;
  reduction_type_t _type;

  void setType(reduction_type_t type) { _type = type; }
  void addDim(std::string dim) { _reduction_dims.push_back(dim); }
  size_t getNumReductionDims() { return _reduction_dims.size(); }
  std::string &getReductionDim(size_t i) {
    if (i < _reduction_dims.size()) {
      return _reduction_dims[i];
    }
  }
  reduction_type_t getReductionType() { return _type; }
};

class IRNode {
public:
  IRNode(irnode_type_t type) : _type(type) { _n_uid = global_n_id++; }
  virtual ~IRNode() = default;

  irnode_type_t getType() const { return _type; }
  irnode_id_t getId() const { return _n_uid; }

private:
  irnode_type_t _type;
  irnode_id_t _n_uid;
};

using IRNodeList = std::vector<irnode_id_t>;

class DataIRNode : public IRNode {
  // private:
  std::string _name;
  DataShape _shape;

public:
  DataIRNode(std::string &name, DataShape shape)
      : IRNode(IRNode_Data), _name(std::string(name)),
        _shape(std::move(shape)) {}

  std::string &getName() { return _name; }
  // ExprAST *getInitVal() { return initVal.get(); }
  DataShape &getShape() { return _shape; }
};

// class DataRefIRNode : public IRNode {
// private:
//   std::string _name;
//   DataShape _shape;

// public:
// };

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
  MemIRNode(std::string &obj, mem_access_mode_t mode, mem_access_type_t type)
      : IRNode(IRNode_Mem), _obj(std::string(obj)), _mode(mode), _type(type) {
    if (_mode == READ) {
      _mode_str = std::string("READ");
    } else if (_mode == WRITE) {
      _mode_str = std::string("WRITE");
    } else if (_mode == READ_WRITE) {
      _mode_str = std::string("READ_WRITE");
    }
  }

  mem_access_type_t getMemAccessType() { return _type; }
  void setAccessMode(mem_access_mode_t mode) { _mode = mode; }
  mem_access_mode_t getAccessMode() { return _mode; }
  std::string &getAccessModeString() { return _mode_str; }

private:
  std::string _obj;
  mem_access_mode_t _mode;
  std::string _mode_str;
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
  ParaShape _para_shape;

public:
  ParaIRNode(std::unique_ptr<IRNodeList> body, ParaShape para_shape)
      : IRNode(IRNode_Parallel), _body(std::move(body)),
        _para_shape(std::move(para_shape)) {}
  ParaIRNode(ParaShape para_shape)
      : IRNode(IRNode_Parallel), _para_shape(std::move(para_shape)) {
    _body = std::make_unique<IRNodeList>();
  }

  void addBodyNode(irnode_id_t node) { _body->push_back(node); }
  IRNodeList *getBody() { return _body.get(); }
  ParaShape &getParaShape() { return _para_shape; }
};

class CtrlIRNode : public RegionIRNode {};

class ForIRNode : public CtrlIRNode {};

class BranchIRNode : public CtrlIRNode {};

#endif // IRNODE_H_