# import igraph 
# from igraph import *

from collections import defaultdict
from irnode import *

class irgraph():
    _nodes = list()
    _irnode_id_to_idx_map = dict()

    _edges = defaultdict()
    _reverse_edges = defaultdict()

    def add_node(self, node):
        node_id = node.get_id()
        self._irnode_id_to_idx_map[node_id] = len(self._nodes)
        self._nodes.append(node)
        return node_id
    
    def add_edge(self, src, dest):
        self._edges[src] = dest
        self._reverse_edges[dest] = src

    def get_node(self, node_id):
        return self._nodes[self._irnode_id_to_idx_map[node_id]]

    def get_nodes(self):
        return self._nodes

    def get_edges(self):
        return self._edges
    
    def get_node_ids_without_in_edges(self):
        node_id_list = []
        for node_id, idx in self._irnode_id_to_idx_map.items():
            if node_id not in self._reverse_edges.keys():
                node_id_list.append(node_id)
        return node_id_list

    def get_src_node_ids(self, node_id):
        node_id_list = []
        if node_id in self._reverse_edges.keys():
            ret = self._reverse_edges[node_id]
            if isinstance(ret, int):
                node_id_list.append(ret)
            elif isinstance(ret, list):
                node_id_list += ret
        return node_id_list
    
    def get_dest_node_ids(self, node_id):
        node_id_list = []
        if node_id in self._edges.keys():
            ret = self._edges[node_id]
            if isinstance(ret, int):
                node_id_list.append(ret)
            elif isinstance(ret, list):
                node_id_list += ret
        return node_id_list

class irgraph_dumper():
    def __init__(self):
        self._f = None

    def dump_node(self, node_id, label, shape, font_size):
        self._f.write('  ' + str(node_id) + ' [' \
                      + 'label=\"' + label + '\", ' \
                      + 'shape=' + shape + ', ' \
                      + 'fontsize=' + str(font_size) \
                      + '];\n')
        
    def dump_edge(self, s, d):
        self._f.write('  ' + str(s) + ' -> ' + str(d) + ';\n' )

    def dump_datanode(self, dn: data_irnode):
        
        shape = dn.get_shape()
        label = '<'

        if len(shape) > 0:
            for i in range(len(shape)-1):
                dim = shape[i]
                label += str(dim) + ', '
            label += str(shape[-1])

        label += '>'

        self.dump_node(dn.get_id(), label, 'ellipse', 9)

    def dump_binnode(self, bn: binop_irnode):
        self.dump_node(bn.get_id(), bn.get_op(), 'box', 9)

    def dump(self, node: irnode):
        if isinstance(node, data_irnode):
            self.dump_datanode(node)
        elif isinstance(node, binop_irnode):
            self.dump_binnode(node)

    def dump_graph(self, irgraph: irgraph):
        self._f = open('graph.dot', 'w')
        # with open('graph.dot', 'w') as self._f:
        
        self._f.write('digraph G {\n')

        nodes = irgraph.get_nodes()
        for node in nodes:
            print(node)
            self.dump(node)
        
        edges = irgraph.get_edges()
        for s, d in edges.items():
            self.dump_edge(s, d)

        self._f.write('}\n')

        self._f.flush()
        self._f.close()
