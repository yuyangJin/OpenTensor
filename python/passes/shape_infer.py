from irgraph import *
from irnode import *


def bin_irnode_shape_inference(g, node):
    src_node_ids = g.get_src_node_ids(node.get_id())
    print(src_node_ids)
    for src_node_id in src_node_ids:
        node.add_input(g.get_node(src_node_id))
        print("-----------add_input")
    # TODO: check inputs with same shape?

    # set output shape the same as input shape
    input_shape = node.get_input_shapes()
    node.set_output_shapes([node.get_input_shapes()[0]])

def slice_irnode_shape_inference(g, node):
    n_slice = node.get_slice()
    shape = []
    for d_slice in n_slice:
        shape.append(d_slice[1] - d_slice[0])
    return shape

def data_irnode_shape_inference(g, node):
    if node.get_shape() != []:
        print(node.get_name(), node.get_shape())
        return
    src_node_ids = g.get_src_node_ids(node.get_id())
    for src_node_id in src_node_ids:
        src_node = g.get_node(src_node_id)
        if isinstance(src_node, sliceop_irnode):
            print("------------data-slice")
            shape = slice_irnode_shape_inference(g, src_node)
            node.set_shape(shape)
            return 
        shape = src_node.get_output_shapes()
        if len(shape) > 0:
            node.set_shape(shape[0])
            print(node.get_name(), shape[0])
            return 


def shape_inference(graph, node):
    if isinstance(node, binop_irnode):
        bin_irnode_shape_inference(graph, node)
    elif isinstance(node, data_irnode):
        data_irnode_shape_inference(graph, node)