#ifndef IRGRAPH_H_
#define IRGRAPH_H_

#include <vector>
#include <memory>
#include "IR/irnode.h"

class IRGraph {
private:
    std::vector<std::unique_ptr<IRNode>> nodes;
    std::vector<std::pair<std::unique_ptr<IRNode>, std::unique_ptr<IRNode>>> edges;

public:
    IRGraph(){}
    // void addNode();
    // IRNodeList* getInNode();
    // IRNodeList* getOutNode();
};


class SpmdIRGraph :public IRGraph {

};

#endif