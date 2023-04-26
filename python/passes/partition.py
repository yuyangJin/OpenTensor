from irgraph import *
from irnode import *
import matplotlib.pyplot as plt
from matplotlib import colors
import numpy as np

class Partition(object):
    def __init__(self, node):
        self._node = node

    '''
    dim : Dimension 
    n : divide into n part

    dim=1, n=2: 
    [(0,9)] -> [[(0,5)],[(6,10)]]
    [(0,9),(0,9)] -> [[(0,5),(0,10)],[(6,10),(0,10)]]

    '''
    def cart(self, dim, n):
        shape = None
        idx = None
        if isinstance(self._node, data_irnode):
            shape = self._node.shape()
            idx = self._node.idx()
        elif isinstance(self._node, op_irnode):
            shapes = self._node.get_input_shapes()
            if len(shapes) > 0:
                shape = shapes[0]

            nodes = self._node.get_input_nodes()
            if len(nodes) > 0:
                idx = nodes[0].idx()

        print(idx)

        if len(shape) < dim:
            print("Error: dim > the dimension of shape")
            return

        l = shape[dim-1]

        n_l = int((l+1) / n)

        for i in range(n - 1):
            pi_idx = []
            for j in range(len(shape)):
                if j != dim-1:
                    pi_idx.append((0,shape[j]))
                else:
                    pi_idx.append((i * n_l, (i+1) * n_l))
            # part_idx.append([])
            self._node.add_partition_idx(pi_idx)

        pi_idx = []
        for j in range(len(shape)):
            if j != dim-1:
                pi_idx.append((0,shape[j]))
            else:
                pi_idx.append(((n-1) * n_l, l))
        self._node.add_partition_idx(pi_idx)

        # self._node.set_partition_idx()
    
        print(self._node.partition_idxs())

    def visualize_inputs(self):
        if isinstance(self._node, op_irnode):
            nodes = self._node.get_input_nodes()
            print(len(nodes))
            figs, axs = plt.subplots(len(nodes), figsize=(2, len(nodes) * 2))
            print(axs)
            for i in range(len(nodes)):
                node = nodes[i]

                shape = node.shape()
                p_idxs = node.partition_idxs() # [[(0, 5), (0, 19)], [(5, 10), (0, 19)]]

                data = np.zeros(tuple(node.shape()))

                print('data:', data.shape)
                
                for j in range(len(p_idxs)):
                    pi = p_idxs[j]
                    slice_idxs = list()
                    for idx in pi:
                        slice_idxs.append(slice(*idx))
                    data[tuple(slice_idxs)] = j
                    print(data)
                
                print(data)

                cdict = {
                    'red'  :  ( (0.0, 0.25, .25), (0.02, .59, .59), (1., 1., 1.)),
                    'green':  ( (0.0, 0.0, 0.0), (0.02, .45, .45), (1., .97, .97)),
                    'blue' :  ( (0.0, 1.0, 1.0), (0.02, .75, .75), (1., 0.45, 0.45))
                }

                cmap = colors.LinearSegmentedColormap('my_colormap', cdict, 1024)


                axs[i].pcolormesh(data, cmap=cmap, vmin=0, vmax=len(p_idxs), edgecolor='k')
                # for x in range(shape[0] + 1):
                # # axs[i].grid()
                #     axs[i].axhline(x, lw=2, color='k', zorder=5)
                # for x in range(shape[1] + 1):
                #     axs[i].axvline(x, lw=2, color='k', zorder=5)
                axs[i].axis('off')

            plt.show()
