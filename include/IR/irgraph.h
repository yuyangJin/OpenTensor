#ifndef IRGRAPH_H_
#define IRGRAPH_H_

#include "IR/irnode.h"
#include "dbg.h"
#include <fstream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

using edge_t = std::pair<irnode_id_t, irnode_id_t>;

class IRGraph {
private:
  std::vector<std::shared_ptr<IRNode>> _nodes;
  // std::vector<std::unique_ptr<IRNode>> _nodes;
  // std::vector<IRNode*> _nodes;
  std::unordered_map<irnode_id_t, int> _irnode_id_to_idx_map;

  std::vector<edge_t> _edges;

public:
  IRGraph() {}
  irnode_id_t addNode(std::shared_ptr<IRNode> node);
  // irnode_id_t addNode(std::unique_ptr<IRNode> node);
  // irnode_id_t addNode(IRNode* node);
  void addEdge(irnode_id_t src, irnode_id_t dest);
  IRNode *getNode(irnode_id_t id);
  std::vector<std::shared_ptr<IRNode>> &getNodes() { return _nodes; }
  // std::vector<std::unique_ptr<IRNode>>& getNodes() {return _nodes;}
  // std::vector<IRNode*>& getNodes() {return _nodes;}
  std::vector<edge_t> &getEdges() { return _edges; }

  // void dumpGraph();
  // void dumpEdge(std::ofstream& fs, std::pair<irnode_id_t, irnode_id_t>&
  // edge); IRNodeList* getInNode(); IRNodeList* getOutNode();
};

class SpmdIRGraph : public IRGraph {};

class IRGraphDumper {
public:
  IRGraphDumper();
  ~IRGraphDumper();
  void dump(IRGraph *graph);

private:
  std::ofstream _fs;
  void dump(IRNode *n);
  void dump(DataIRNode *dn);
  void dump(CallIRNode *cn);
  void dump(MemIRNode *mn);
  void dump(TaskIRNode *tn);
  void dump(EinsumTaskIRNode *etn);
  void dump(CommIRNode *cn);
  void dump(ParaIRNode *pn);
  void dump(ForIRNode *fn);
  void dump(BranchIRNode *bn);
  // void dump(edge_t& e);
  void dumpNode(irnode_id_t id, std::string &name, std::string &shape);
  void dumpNode(irnode_id_t id, std::string &name, std::string &shape,
                int font_size);
  void dumpEdge(edge_t &edge);
  void dumpRegion(irnode_id_t id, std::string &name, IRNodeList *body,
                  std::string &color);
};

#endif