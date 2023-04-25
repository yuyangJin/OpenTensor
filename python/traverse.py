from irgraph import *
from irnode import *


class Traversal(object) :
    def __init__(self, irgraph, func):
        self._graph = irgraph
        self._inference_nodelist = list()
        self._accessed_nodelist = list()
        self._waiting_nodelist = list()
        self.traverse(func)

    def is_nodes_accessed(self, node_ids):
        all_nodes_are_accessed = True
        for node_id in node_ids:
            if node_id not in self._accessed_nodelist:
                all_nodes_are_accessed = False
                break
        return all_nodes_are_accessed

    def is_src_nodes_accessed(self, node_id) :
        src_node_ids = self._graph.get_src_node_ids(node_id)
        return self.is_nodes_accessed(src_node_ids)

    def traverse(self, func):
        nodes_without_in_edge = self._graph.get_node_ids_without_in_edges()

        for node_id in nodes_without_in_edge:
            self._inference_nodelist.append(node_id)
            self._accessed_nodelist.append(node_id)
        
        while len(self._inference_nodelist) > 0 :
            node_id = self._inference_nodelist[0]
            self._inference_nodelist.remove(node_id)
            node = self._graph.get_node(node_id)

            ''' Execute the input function for each node '''
            func(self._graph, node)
            
            dest_nodes = self._graph.get_dest_node_ids(node_id)
            for dest_node_id in dest_nodes:
                if dest_node_id in self._accessed_nodelist:
                    continue
                else:
                    # insert related nodes into accessed nodelist
                    # dest_node = self._graph.get_src_nodes(dest_node_id)
                    self._accessed_nodelist.append(dest_node_id)
                    if (self.is_src_nodes_accessed(dest_node_id)):
                        self._inference_nodelist.append(dest_node_id)
                    else:
                        self._waiting_nodelist.append(dest_node_id)
            for i in range(len(self._waiting_nodelist)):
                waiting_node_id = self._waiting_nodelist[i]
                if (self.is_src_nodes_accessed(waiting_node_id)):
                    self._waiting_nodelist.remove(waiting_node_id)
                    self._inference_nodelist.append(waiting_node_id)
                    #  do  --i, because remove return the iterator of next element
                    i -= 1


