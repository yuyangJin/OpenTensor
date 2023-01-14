#include "IR/irgraph.h"

IRGraphDumper::IRGraphDumper() {
    _fs.open("graph.dot", std::ios::out);
}

IRGraphDumper::~IRGraphDumper() {
    _fs.close();
}

void IRGraphDumper::dump(IRGraph& graph) {
    _fs << "digraph G {" << std::endl;

    auto& nodes = graph.getNodes();

    for (long unsigned int i = 0; i < nodes.size(); i++) {
        auto node = nodes[i].get();
        dump(node);
    }

    auto& edges = graph.getEdges();
    for (auto& edge : edges) {
        dumpEdge(edge);
    }
    
    _fs << "}";
}

void IRGraphDumper::dumpEdge(edge_t& edge) {
    _fs << edge.first << " -> " << edge.second << ";" << std::endl;
}
/**
 * @brief Standard output for 
 * 
 */
void IRGraphDumper::dumpNode(irnode_id_t id, std::string& name, std::string& shape) {
    _fs << "  " << id << " [";
    _fs << "label=\"" << name << "\"";
    _fs << "shape=\"" << shape << "\"";
    _fs << "  " << id << "]\n";
}

// void IRGraphDumper::dumpRegion() {

// }

void IRGraphDumper::dump(IRNode* n) {
    auto node_type = n->getType();
    switch(node_type) {
        case IRNode_Data: {
            auto* dn = dynamic_cast<DataIRNode *>(n);
            dump(dn);
            break;
        }
        case IRNode_Call: {
            auto* cn = dynamic_cast<CallIRNode *>(n);
            dump(cn);
            break;
        }
        case IRNode_Mem: {
            auto* mn = dynamic_cast<MemIRNode *>(n);
            dump(mn);
            break;
        }
        case IRNode_Task: {
            auto* tn = dynamic_cast<TaskIRNode *>(n);
            dump(tn);
            break;
        }
        case IRNode_EinsumTask: {
            auto* etn = dynamic_cast<EinsumTaskIRNode *>(n);
            dump(etn);
            break;
        }
        case IRNode_Comm: {
            auto* cn = dynamic_cast<CommIRNode *>(n);
            dump(cn);
            break;
        }
        case IRNode_Parallel: {
            auto* pn = dynamic_cast<ParaIRNode *>(n);
            dump(pn);
            break;
        }
        case IRNode_For: {
            auto* fn = dynamic_cast<ForIRNode *>(n);
            dump(fn);
            break;
        }
        case IRNode_Branch: {
            auto* bn = dynamic_cast<BranchIRNode *>(n);
            dump(bn);
            break;
        }
        default:
            break;
    }
}

void IRGraphDumper::dump(DataIRNode* dn) {
    std::string shape = std::string("circle");
    dumpNode(dn->getId(), dn->getName(), shape);
}
void IRGraphDumper::dump(CallIRNode* cn) {
    std::string shape = std::string("box");
    dumpNode(cn->getId(), cn->getCalleeFuncName(), shape);
}
void IRGraphDumper::dump(MemIRNode* mn) {
    std::string shape = std::string("box");
    dumpNode(mn->getId(), mn->getAccessModeString(), shape);
}
void IRGraphDumper::dump(TaskIRNode* tn) {}
void IRGraphDumper::dump(EinsumTaskIRNode* etn) {
    std::string shape = std::string("octagon");
    std::string name = etn->getLHS() + " = " + etn->getRHS();
    dumpNode(etn->getId(), name, shape);
}
void IRGraphDumper::dump(CommIRNode* cn) {}
void IRGraphDumper::dump(ParaIRNode* pn) {}
void IRGraphDumper::dump(ForIRNode* fn) {}
void IRGraphDumper::dump(BranchIRNode* bn) {}