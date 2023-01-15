#include "IR/irgraph.h"

irnode_id_t IRGraph::addNode(std::shared_ptr<IRNode> node) {
  // irnode_id_t IRGraph::addNode(std::unique_ptr<IRNode> node) {
  // irnode_id_t IRGraph::addNode(IRNode* node) {
  _irnode_id_to_idx_map[node->getId()] = _nodes.size();
  auto node_id = node->getId();
  // dbg(node->getId());

  std::shared_ptr<IRNode> new_node(node);
  // dbg(node);
  // dbg(new_node.get());
  _nodes.emplace_back(new_node);
  return node_id;
}

void IRGraph::addEdge(irnode_id_t src, irnode_id_t dest) {
  _edges.emplace_back(src, dest);
}

IRNode *IRGraph::getNode(irnode_id_t id) {
  return _nodes[_irnode_id_to_idx_map[id]].get();
  // return _nodes[_irnode_id_to_idx_map[id]];
}
