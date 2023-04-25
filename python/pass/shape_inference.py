from irgraph import *
from irnode import *










def bin_irnode_shape_inference(node):
    print(node.get_id(), 'bin_node')

def data_irnode_shape_inference(node):
    print(node.get_id(), 'data_node')



def shape_inference(node):
    if isinstance(node, bin_irnode):
        bin_irnode_shape_inference(node)
    elif isinstance(node, data_irnode):
        data_irnode_shape_inference(node)