import ast
from ast import *
import sys
# import TensorIR as tir
# from TensorIR import *

import irgraph as tir
from irgraph import *
from irnode import *
from traverse import *
from passes.shape_inference import *
from passes.partition import *


class ASTTraversal(object) :
    def __init__(self, ast):
        self._ast = ast
        self._irgraph = tir.irgraph()
        self._op_asname = ''
        self._tensor_to_node = dict()

    def traverse_Import(self, node):
        for import_name in node.names:
            if import_name.name == 'opentensor':
                self._op_asname = import_name.asname

    def traverse_Call(self, node):
        if node.func.value.id == self._op_asname:
            arg_list = []
            for arg in node.args:
                if isinstance(arg, Set):
                    arg_list.append([])
                    for elt in arg.elts:
                        if isinstance(elt, Name):
                            arg_list[-1].append(elt.id)
                        elif isinstance(elt, Constant):
                            arg_list[-1].append(elt.value)
                elif isinstance(arg, Name):
                    arg_list.append(arg.id)
                elif isinstance(arg, Constant):
                    arg_list.append(arg.value)
            ''' define a tensor '''
            if node.func.attr == 'tensor':
                dnode = tir.data_irnode('?', arg_list[0])
                dnode_id = self._irgraph.add_node(dnode)
                # print(dnode, sys.getrefcount(dnode))
                return dnode, arg_list
            elif node.func.attr == 'add':
                bnode = tir.binop_irnode('+')
                bnode_id = self._irgraph.add_node(bnode)
                # print(bnode, sys.getrefcount(bnode))
                return bnode, arg_list
            
        return None, arg_list

    def traverse_Assign(self, node):
        lhs_node = None
        rhs_node = None

        lhs_id = -1
        rhs_id = -1


        lhs_name = None
        for target in node.targets:
            lhs_name = target.id
        if isinstance(node.value, Call):
            rhs_node, input_tensors = self.traverse_Call(node.value)
            if rhs_node != None:
                rhs_id = rhs_node.get_id()
                
                if rhs_node.get_type() == irnode_type.IRNode_Data:
                    lhs_node = rhs_node
                    lhs_node.set_name(lhs_name)
                    self._tensor_to_node[lhs_name] = lhs_node
                    return 
                elif rhs_node.get_type() == irnode_type.IRNode_BinOp:
                    lhs_node = tir.data_irnode(lhs_name, [])
                    lhs_id = self._irgraph.add_node(lhs_node)

                    for t in input_tensors:
                        t_node = self._tensor_to_node[t]
                        t_node_id = t_node.get_id()
                        self._irgraph.add_edge(t_node_id, rhs_id)
                    
                

                if lhs_id != -1 and rhs_id != -1:
                    self._irgraph.add_edge(rhs_id, lhs_id)
                


    def traverse_AST(self, node):
        for child in ast.iter_child_nodes(node):
            if isinstance(child, Import):
                self.traverse_Import(child)
            if isinstance(child, Assign):
                self.traverse_Assign(child)

            # if isinstance(child, Call):
            #     self.traverse_Call(child)
        

        # for child in ast.iter_child_nodes(node):
        #     self.traverse_AST(child)

    def traverse(self):
        self.traverse_AST(self._ast)
        return self._irgraph

    # def get_irgraph(self):
    #     return self._irgraph





with open(sys.argv[1], 'r') as fp:
    data = fp.readlines()

data = ''.join(data)
_ast = ast.parse(data)



visitor = ASTTraversal(_ast)
irg = visitor.traverse()

# print(irg.get_nodes())
print(irg.get_edges())
print(irg.get_reverse_edges())



gt = Traversal(irg, shape_inference)

Partition(irg.get_node(1)).cart(1, 2)
Partition(irg.get_node(2)).cart(1, 2)

Partition(irg.get_node(3)).visualize_inputs()

# dumper = tir.IRGraphDumper()
dumper = tir.irgraph_dumper()
dumper.dump_graph(irg)

print(ast.dump(_ast), sep='\n')
