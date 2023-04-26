import irgraph as tir
from irgraph import *
from irnode import *
from passes.partition import *
from traverse import *
from passes.idx_infer import *
from passes.shape_infer import *

if __name__ == '__main__':

    ''' Test 1: IRGraph construction '''
    irg = tir.irgraph()

    ''' Test 2: Create IRNode '''
    # Data
    dn1 = tir.data_irnode('A', [10, 10])
    dn2 = tir.data_irnode('B')
    dn3 = tir.data_irnode('C')
    dn4 = tir.data_irnode('D')


    # SliceOp
    sn1 = tir.sliceop_irnode([(1,10),(0,9)])
    sn2 = tir.sliceop_irnode([(1,10),(1,10)])

    # BinOp
    bn = tir.binop_irnode('+')

    ''' Test 3: Add IRNode into IRGraph '''
    dn1_id = irg.add_node(dn1)
    dn2_id = irg.add_node(dn2)
    dn3_id = irg.add_node(dn3)
    dn4_id = irg.add_node(dn4)
    sn1_id = irg.add_node(sn1)
    sn2_id = irg.add_node(sn2)
    bn_id = irg.add_node(bn)



    ''' Test 4: Add edges into IRGraph'''
    irg.add_edge(dn1_id, sn1_id)
    irg.add_edge(dn1_id, sn2_id)
    irg.add_edge(sn1_id, dn2_id)
    irg.add_edge(sn2_id, dn3_id)
    irg.add_edge(dn2_id, bn_id)
    irg.add_edge(dn3_id, bn_id)
    irg.add_edge(bn_id, dn4_id)





    Partition(dn1).cart(2, 3)


    Traversal(irg, shape_inference)

    Traversal(irg, idx_infer)


    print("dn2:", dn2.partition_idxs() )
    print('dn3', dn3.partition_idxs() )


    Partition(bn).visualize_inputs()








    ''' Test 5: GraphDumper'''
    dumper = tir.irgraph_dumper()
    dumper.dump_graph(irg)