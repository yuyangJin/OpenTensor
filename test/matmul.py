import opentensor as ot
A = ot.rand(3, 4) 
B = ot.rand(4, 5)
C = ot.zeros(3, 5)

# i, j, k = ot.dim()
# @einsum
# def mm(A, B, C):
'''einsum'''
C[i, j] = (A[i, k] * B[k, j]).sum(k)

# mm(A, B, C)
print(C)