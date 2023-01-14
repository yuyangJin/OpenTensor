#include "IR/irgraph.h"


void IRGraph::addNode(std::unique_ptr<IRNode> node) {
    _irnode_id_to_idx_map[node->getId()] = _nodes.size();
    _nodes.emplace_back(std::move(node));
}

void IRGraph::addEdge(std::unique_ptr<IRNode> src, std::unique_ptr<IRNode> dest) {
    _edges.emplace_back(src->getId(), dest->getId());
}

IRNode* IRGraph::getNode(irnode_id_t id) {
    return _nodes[_irnode_id_to_idx_map[id]].get();
}

