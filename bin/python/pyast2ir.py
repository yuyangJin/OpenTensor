import ast
from ast import *
import sys
import TensorIR as tir
from TensorIR import *


class ASTVisitor(ast.NodeVisitor):
    _op_asname = ''

    def visit_Import(self, node):
        for import_name in node.names:
            if import_name.name == 'opentensor':
                self._op_asname = import_name.asname

    def visit_Assign(self, node):
        for target in node.targets:
            print(target.id)
        print('=')
        self.generic_visit(node)

    def visit_Call(self, node):
        if node.func.value.id == self._op_asname:
            print(node.func.attr)
            for arg in node.args:
                if isinstance(arg, Set):
                    for elt in arg.elts:
                        if isinstance(elt, Name):
                            print(elt.id)
                        elif isinstance(elt, Constant):
                            print(elt.value)
            self.generic_visit(node)

    # def visit_Call(self, node):


if __name__ == '__main__':

    with open(sys.argv[1], 'r') as fp:
        data = fp.readlines()

    data = ''.join(data)
    _ast = ast.parse(data)

    visitor = ASTVisitor()
    visitor.visit(_ast)

    print(ast.dump(_ast), sep='\n')

    
    irgraph = tir.IRGraph()

    ds = tir.DataShape()
    ds.add_dim(3)
    ds.add_dim(4)
    dn1 = tir.DataIRNode('A', ds)

    dn2 = tir.DataIRNode('B', ds)

    dn1_id = irgraph.add_datanode(dn1)
    dn2_id = irgraph.add_datanode(dn2)

    bn3 = tir.BinIRNode('+')

    bn3_id = irgraph.add_binnode(bn3)

    irgraph.add_edge(dn1_id, dn2_id)
    irgraph.add_edge(dn1_id, bn3_id)
    irgraph.add_edge(dn2_id, bn3_id)

    dumper = tir.IRGraphDumper()
    dumper.dump(irgraph)
    print('end')