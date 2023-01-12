#include "opentensor.h"

int main() {
  Tensor A;
  A.ot_random(3 * 4);
  Tensor B;
  B.ot_random(4 * 5);
  Tensor C;
  C.ot_zeros(3 * 5);

  Dim i, j, k;
  // C["ij"] = (A["ik"] * B["kj"]);//.sum(k);
  C[i][j].einsum((A[i][k] * B[k][j]).sum(k));
}
