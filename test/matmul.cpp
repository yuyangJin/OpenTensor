#include "opentensor.h"

int main() {
  Tensor A(3, 4);
  A.random();
  Tensor B(4, 5);
  B.random();
  Tensor C(3, 5);
  C.zeros();

  Dim i, j, k;
  // C["ij"] = (A["ik"] * B["kj"]);//.sum(k);
  C[i][j].einsum((A[i][k] * B[k][j]).sum(k));

  C.print();
}
