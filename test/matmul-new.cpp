#include "einsum.h"

int main() {
  Tensor A(512, 128);
  A.random();
  Tensor B(128, 256);
  B.random();
  Tensor C(512, 256);
  C.zeros();

//   Dim i, j, k;
//   C[i][j].einsum((A[i][k] * B[k][j]).sum(k));
#pragma einsum
  C.einsum("ik,kj->ij", A, B);

  C.print();
}
