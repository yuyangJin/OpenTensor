import enum
from enum import Enum

global_node_id = 0

class irnode_type(Enum) :
    IRNode_Data = 0
    IRNode_Op = 100
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

class op_irnode(irnode):
    def __init__(self, type):
        super(op_irnode, self).__init__(type)
        self._num_inputs = 0
        self._num_outputs = 0
        self._input_shapes = []
        self._output_shapes = []

    def set_num_inputs(self, n):
        self._num_inputs = n

    def set_num_outputs(self, n):
        self._num_outputs = n

    def set_input_shapes(self, s):
        self._input_shapes = s

    def set_output_shapes(self, s):
        self._output_shapes = s

    def get_input_shapes(self):
        return self._input_shapes

    def get_output_shapes(self):
        return self._output_shapes

    def add_input(self, n):
        if isinstance(n, data_irnode):
            self._num_inputs += 1
            self._input_shapes.append(n.get_shape())

    

class binop_irnode(op_irnode):
    def __init__(self, op):
        # super(binop_irnode, self).__init__(irnode_type.IRNode_BinOp)
        op_irnode.__init__(self, irnode_type.IRNode_BinOp)
        self._op = op
    
    def get_op(self):
        return self._op

        