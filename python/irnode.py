import enum
from enum import Enum

global_node_id = 0

class irnode_type(Enum) :
    IRNode_Data = 0
    IRNode_BinOp = 1
    IRNode_SliceOp = 2
    IRNode_Call = 3
    IRNode_Mem = 4
    IRNode_Task = 5
    IRNode_Comm = 6
    IRNode_Region = 7
    IRNode_Parallel = 8
    IRNode_For = 9
    IRNode_Branch = 10

class irnode(object) : 
    _type: irnode_type
    _node_id: int
    _region_node_id: int

    def __init__(self, type):
        global global_node_id
        self._type = type
        global_node_id += 1
        self._node_id = global_node_id
        self._region_node_id = -1 # -1 stands for this node has no outer region

    def get_type(self):
        return self._type
    
    def get_id(self):
        return self._node_id
    
class data_irnode(irnode):
    _name = ''
    _shape = None

    def __init__(self, name):
        super(data_irnode, self).__init__(irnode_type.IRNode_Data)
        self._name = name
        self._shape = list()

    def __init__(self, name, shape):
        super(data_irnode, self).__init__(irnode_type.IRNode_Data)
        self._name = name
        self._shape = shape

    def set_name(self, name):
        self._name = name

    def set_shape(self, shape):
        self._shape = shape

    def add_dim(self, dim):
        self._shape.append(dim)

    def get_name(self):
        return self._name
    
    def get_shape(self):
        return self._shape


class bin_irnode(irnode):
    _op = ''

    def __init__(self, op):
        super(bin_irnode, self).__init__(irnode_type.IRNode_BinOp)
        self._op = op
    
    def get_op(self):
        return self._op

        