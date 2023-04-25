from irgraph import *
from irnode import *


def bin_irnode_shape_inference(g, node):
    src_node_ids = g.get_src_node_ids(node.get_id())
    for src_node_id in src_node_ids:
        node.add_input(g.get_node(src_node_id))
    # TODO: check inputs with same shape?

    # set output shape the same as input shape
    input_shape = node.get_input_shapes()
    node.set_output_shapes([node.get_input_shapes()[0]])

    

def data_irnode_shape_inference(g, node):
    if node.get_shape() != []:
        print(node.get_name(), node.get_shape())
        return
    src_node_ids = g.get_src_node_ids(node.get_id())
    for src_node_id in src_node_ids:
        shape = g.get_node(src_node_id).get_output_shapes()
        if len(shape) > 0:
            node.set_shape(shape[0])
            print(node.get_name(), shape[0])
            return 



def shape_inference(graph, node):
    if isinstance(node, binop_irnode):
        bin_irnode_shape_inference(graph, node)
    elif isinstance(node, data_irnode):
        data_irnode_shape_inference(graph, node)