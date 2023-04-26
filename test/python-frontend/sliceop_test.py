import irgraph as tir
from irgraph import *
from irnode import *

if __name__ == '__main__':

    ''' Test 1: IRGraph construction '''
    irg = tir.irgraph()

    ''' Test 2: Create IRNode '''
    # DataIRNode
    dn1 = tir.data_irnode('A', [10])
    dn2 = tir.data_irnode('B')

    # SliceIRNode
    sn = tir.sliceop_irnode([0,7])

    ''' Test 3: Add IRNode into IRGraph '''
    dn1_id = irg.add_node(dn1)
    dn2_id = irg.add_node(dn2)
    sn_id = irg.add_node(sn)

    ''' Test 4: Add edges into IRGraph'''
    irg.add_edge(dn1_id, sn_id)
    irg.add_edge(sn_id, dn2_id)

    print(irg.get_nodes())
    print(irg.get_edges())

    ''' Test 5: GraphDumper'''
    dumper = tir.irgraph_dumper()
    dumper.dump_graph(irg)