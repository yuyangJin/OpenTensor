#ifndef IRNODE_H_
#define IRNODE_H_

#include <string>
#include <vector>

struct DataShape {
    std::vector<int64_t> shape;
};

struct ArgList {
    std::vector<std::string> arg_list;
}

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
        IRNode_Parallel, 
        IRNode_For, 
        IRNode_Branch,
    };
    
    IRNode(irnode_type_t type)
        : _type(type) {}
    virtual ~IRNode() = default;

    irnode_type_t getType() const {return _type;}
private:    
    irnode_type_t _type; 
};

class DataDeclIRNode : public IRNode {
// private:
    std::string _name;
    DataShape _shape;

public:
    DataDeclIRNode(std::string& name, DataShape shape)
        :IRNode(IRNode_DataDecl), _name(std::move(name)), _shape(std::move(shape)) {}

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
    CallIRNode(std::string& func_name, ArgList args)
        :IRNode(IRNode_Call), _callee_func_name(std::move(func_name)), _args(std::move(args)) {}

    std::string &getCalleeFuncName() { return _callee_func_name; }
    ArgList &getArgs() {return _args;}
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

    MemIRNode(std::string obj, mem_access_mode_t mode, mem_access_type_t type)
        :IRNode(IRNode_Mem), _obj(std::move(obj)), _mode(mode), _type(type) {}    

    mem_access_type_t getMemAccessType() {return _type;}
    void setAccessMode(mem_access_mode_t)

private:
    
    std::string& _obj;
    mem_access_mode_t _mode;
    mem_access_type_t _type;

};

class TaskIRNode : public IRNode {

    TaskIRNode() :IRNode(IRNode_Task) {} 
    
};

class EinsumTaskIRNode : public IRNode {
public:
    enum reduction_type_t {
        SUM,
        MAX,
        MIN,
        AVG,
        NONE,
    };
    EinsumTaskIRNode(reduction_type_t reduction_type) 
        : IRNode(IRNode_EinsumTask), _reduction_type(_reduction_type), {} 
private:
    reduction_type_t _reduction_type;
};

class CommIRNode : public IRNode {

};

class RegionIRNode : public IRNode {

};

class ParaIRNode : public RegionIRNode {

};

class CtrlIRNode : public RegionIRNode {

};

class ForIRNode : public CtrlIRNode {

};

class BranchIRNode : public CtrlIRNode {

};


#endif // IRNODE_H_