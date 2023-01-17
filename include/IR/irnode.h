#ifndef IRNODE_H_
#define IRNODE_H_

#include <memory>
#include <string>
#include <vector>

using irnode_id_t = size_t;

static int global_node_id = 0;

struct DataShape {
  std::vector<int64_t> _shape;

  size_t getNumDims() { return _shape.size(); }
  int64_t getDim(size_t i) { return _shape[i]; }
};

struct ParaShape {
  std::vector<std::string> _dims;
  std::vector<int64_t> _shape;

  size_t getNumDims() { return _dims.size(); }
  std::string &getDim(size_t i) { return _dims[i]; }
  int64_t getDimLen(size_t i) { return _shape[i]; }
};

struct ArgList {
  std::vector<std::string> _args;

  void addArg(std::string arg) { _args.push_back(arg); }
  size_t getNumArgs() { return _args.size(); }
  std::string &getArg(size_t i) { return _args[i]; }
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
  std::vector<int64_t> _reduction_shape;
  reduction_type_t _type;

  void setType(reduction_type_t type) { _type = type; }
  void addDim(std::string dim) { _reduction_dims.push_back(dim); }
  void addShape(int64_t len) { _reduction_shape.push_back(len); }
  size_t getNumReductionDims() { return _reduction_dims.size(); }
  std::string &getReductionDim(size_t i) { return _reduction_dims[i]; }
  int64_t &getReductionShape(size_t i) { return _reduction_shape[i]; }
  reduction_type_t getReductionType() { return _type; }
};

class IRNode {
public:
  IRNode(irnode_type_t type) : _type(type) {
    _node_id = global_node_id++;
    _region_node_id = -1; // -1 stands for this node has no outer region
  }
  virtual ~IRNode() = default;

  irnode_type_t getType() const { return _type; }
  irnode_id_t getId() const { return _node_id; }
  void setRegionNode(irnode_id_t region_node_id) {
    _region_node_id = region_node_id;
  }
  bool hasRegionNode() {
    if (_region_node_id != (irnode_id_t)-1) {
      return true;
    } else {
      return false;
    }
  }
  irnode_id_t getRegionNode() const { return _region_node_id; }

private:
  irnode_type_t _type;
  irnode_id_t _node_id;
  irnode_id_t _region_node_id;
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
  std::string _obj_name;
  std::string _callee_func_name;
  ArgList _args;

public:
  CallIRNode(std::string &obj_name, std::string &func_name, ArgList args)
      : IRNode(IRNode_Call), _obj_name(obj_name),
        _callee_func_name(std::string(func_name)), _args(std::move(args)) {}

  std::string &getCalleeFuncName() { return _callee_func_name; }
  std::string &getObjName() { return _obj_name; }
  ArgList &getArgs() { return _args; }
};

class MemIRNode : public IRNode {
public:
  MemIRNode(std::string &obj, mem_access_mode_t mode, mem_access_type_t type)
      : IRNode(IRNode_Mem), _obj(std::string(obj)), _mode(mode), _type(type) {
    if (_mode == READ) {
      _mode_str = std::string("read");
    } else if (_mode == WRITE) {
      _mode_str = std::string("write");
    } else if (_mode == READ_WRITE) {
      _mode_str = std::string("read_write");
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
  RegionIRNode(irnode_type_t type) : IRNode(type) {
    _body = std::make_unique<IRNodeList>();
  }
  RegionIRNode(irnode_type_t type, std::unique_ptr<IRNodeList> body)
      : IRNode(type), _body(std::move(body)) {}

  void addBodyNode(irnode_id_t node) { _body->push_back(node); }
  IRNodeList *getBody() { return _body.get(); }
};

class ParaIRNode : public RegionIRNode {
  ParaShape _para_shape;

public:
  ParaIRNode(std::unique_ptr<IRNodeList> body, ParaShape para_shape)
      : RegionIRNode(IRNode_Parallel, std::move(body)),
        _para_shape(std::move(para_shape)) {}
  ParaIRNode(ParaShape para_shape)
      : RegionIRNode(IRNode_Parallel), _para_shape(std::move(para_shape)) {}

  ParaShape &getParaShape() { return _para_shape; }
};

class CtrlIRNode : public RegionIRNode {};

class ForIRNode : public CtrlIRNode {};

class BranchIRNode : public CtrlIRNode {};

#endif // IRNODE_H_