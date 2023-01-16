#include "IR/irgraph.h"

irnode_id_t IRGraph::addNode(std::shared_ptr<IRNode> node) {
  // irnode_id_t IRGraph::addNode(std::unique_ptr<IRNode> node) {
  _irnode_id_to_idx_map[node->getId()] = _nodes.size();
  auto node_id = node->getId();

  std::shared_ptr<IRNode> new_node(node);
  _nodes.emplace_back(new_node);
  return node_id;
}

void IRGraph::addEdge(irnode_id_t src, irnode_id_t dest) {
  _edges.emplace(std::make_pair(src, dest));
  _reverse_edges.emplace(std::make_pair(dest, src));
}

IRNode *IRGraph::getNode(irnode_id_t id) {
  return _nodes[_irnode_id_to_idx_map[id]].get();
}

IRNodeList *IRGraph::getSrcNodes(irnode_id_t id) {
  IRNodeList *src_nodes = new IRNodeList();
  auto start = _reverse_edges.lower_bound(id);
  auto end = _reverse_edges.upper_bound(id);

  for (std::multimap<irnode_id_t, irnode_id_t>::iterator it = start; it != end;
       it++) {
    if (it->first == id) {
      src_nodes->emplace_back(it->second);
    }
  }
  return src_nodes;
}

IRNodeList *IRGraph::getDestNodes(irnode_id_t id) {
  IRNodeList *dest_nodes = new IRNodeList();
  auto start = _edges.lower_bound(id);
  auto end = _edges.upper_bound(id);

  for (std::multimap<irnode_id_t, irnode_id_t>::iterator it = start; it != end;
       it++) {
    if (it->first == id) {
      dest_nodes->emplace_back(it->second);
    }
  }
  return dest_nodes;
}

// /** TODO: Do not use idx as irnode id */
IRNodeList *IRGraph::getNodesWithoutInEdges() {
  IRNodeList *nodes = new IRNodeList();

  for (auto &kv : _irnode_id_to_idx_map) {
    if (_reverse_edges.find(kv.first) ==
        _reverse_edges.end()) { // irnode id is not found as the src node of
                                // reverse edge (same as dest node of edge)
      nodes->emplace_back(kv.first);
    }
  }

  return nodes;
}