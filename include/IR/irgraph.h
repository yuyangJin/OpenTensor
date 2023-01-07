#include <vector>
#include <memory>
#include "IR/irnode.h"

class IRGraph {


private:
    std::vector<std::unique_ptr<IRNode>> nodes;
    std::vector<std::pair<std::unique_ptr<IRNode>, std::unique_ptr<IRNode>>> edges;
};


class SpmdIRGraph :public IRGraph {

};