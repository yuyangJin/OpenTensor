import ast
from ast import *
import sys



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
