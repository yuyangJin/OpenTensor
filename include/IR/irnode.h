#ifndef IRNODE_H_
#define IRNODE_H_

class IRNode {
public:
    enum irnode_type_t {
        IRNode_Data,
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

class DataIRNode : public IRNode {

};

class MemIRNode : public IRNode {

};

class TaskIRNode : public IRNode {

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