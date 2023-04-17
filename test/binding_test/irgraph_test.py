import TensorIR as tir
from TensorIR import *

if __name__ == '__main__':

    ''' Test 1: IRGraph construction '''
    irgraph = tir.IRGraph()

    ''' Test 2: Create IRNode '''
    # DataIRNode
    ds = tir.DataShape()
    ds.add_dim(3)
    ds.add_dim(4)
    dn = tir.DataIRNode('A', ds)

    # SliceIRNode
    sn = tir.SliceIRNode()
    sn.set_slice(0, 1000, 1)

    # BinIRNode
    bn = tir.BinIRNode('+')


    ''' Test 3: Add IRNode into IRGraph '''
    dn_id = irgraph.add_datanode(dn)
    sn_id = irgraph.add_slicenode(sn)
    bn_id = irgraph.add_binnode(bn)


    ''' Test 4: Add edges into IRGraph'''
    irgraph.add_edge(dn_id, sn_id)
    irgraph.add_edge(sn_id, bn_id)

    ''' Test 5: GraphDumper'''
    dumper = tir.IRGraphDumper()
    dumper.dump(irgraph)