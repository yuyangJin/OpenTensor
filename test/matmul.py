import opentensor as ot

A = ot.tensor({3, 4})
B = ot.tensor({3, 4})

C = ot.add(A, B)
# C = ot.add(A[1:-1], B)