#include "opentensor.h"

int main() {
  Tensor R(50, 100);
  R.random();
  Tensor x(100);
  x.random();
  Tensor r(50);
  r.zeros();

  Dim i, j;
  r[i].einsum((R[i][j] * x[j]).sum(j));

  r.print();
}
