#ifndef IRNODE_H_
#define IRNODE_H_

#include <string>
#include <vector>

struct DataShape {
    std::vector<int64_t> shape;
};

class IRNode {
public:
    enum irnode_type_t {
        IRNode_DataDecl,
        IRNode_DataRef,
        IRNode_Mem,
        IRNode_Task,
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

class MemIRNode : public IRNode {

};

class TaskIRNode : public IRNode {
    // std::string ;
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