from irgraph import *
from irnode import *
import copy

''' idx inference and partition idxs inference '''

def binop_irnode_idx_infer(g, node):
    pass
    # src_node_ids = g.get_src_node_ids(node.get_id())
    # print(src_node_ids)
    # for src_node_id in src_node_ids:
    #     node.add_input(g.get_node(src_node_id))
    # # TODO: check inputs with same shape?

    # # set output shape the same as input shape
    # input_shape = node.get_input_shapes()
    # node.set_output_shapes([node.get_input_shapes()[0]])

def sliceop_irnode_idx_infer(g, node):
    print('-------------slice idx reference------------')
    input_node_ids = g.get_src_node_ids(node.get_id())
    if len(input_node_ids) != 1:
        print("Error: the input slicing op is more than 1")
    input_node = g.get_node(input_node_ids[0])
    input_part_idxs = input_node.partition_idxs()
    node_slice = node.get_slice()


    output_part_idxs = copy.deepcopy(input_part_idxs)
    print('input_part_idxs:',input_part_idxs)
    print('node_slice', node_slice)


    for d in range(len(node_slice)):
        d_slice = node_slice[d]
        # print("d_slice", d_slice)
        start = d_slice[0]
        end = d_slice[1]

        for i in range(len(output_part_idxs)):
            idx = output_part_idxs[i]
            # print('idx[d][0]:',idx[d][0])

            idx[d] = tuple([max(idx[d][0], start), min(idx[d][1], end)])
            # idx[d][1] = min(idx[d][1], end)
    print('output_part_idxs', output_part_idxs)
 
    i = 0
    while i < len(output_part_idxs):
        idx = output_part_idxs[i]
        for dim in idx:
            if dim[0] >= dim[1]:
                output_part_idxs.remove(idx)
                break
        i += 1
    print('output_part_idxs', output_part_idxs)
    print('--------------------------------------------')

    return copy.deepcopy(node_slice), output_part_idxs

def data_irnode_idx_infer(g, node):
    src_node_ids = g.get_src_node_ids(node.get_id())
    if len(src_node_ids) == 0:
        return
    elif len(src_node_ids) >= 1:
        print("Error: the src of data node is more than 1")
    src_node = g.get_node(src_node_ids[0])
    if isinstance(src_node, sliceop_irnode):
        print(node.name())
        idx, p_idxs = sliceop_irnode_idx_infer(g, src_node)
        node.set_idx(idx)
        node.set_partition_idxs(p_idxs)
        print(node.partition_idxs())


    # for src_node_id in src_node_ids:
    # if node.get_shape() != []:
    #     print(node.get_name(), node.get_shape())
    #     return
    # src_node_ids = g.get_src_node_ids(node.get_id())
    # for src_node_id in src_node_ids:
    #     shape = g.get_node(src_node_id).get_output_shapes()
    #     if len(shape) > 0:
    #         node.set_shape(shape[0])
    #         print(node.get_name(), shape[0])
    #         return 


def idx_infer(graph, node):
    if isinstance(node, binop_irnode):
        binop_irnode_idx_infer(graph, node)
    elif isinstance(node, data_irnode):
        data_irnode_idx_infer(graph, node)