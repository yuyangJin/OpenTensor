#include "opentensor.h"

int main() {
  Tensor X(3, 4, 5);
  X.random();
  Tensor Y(4, 5, 6);
  Y.random();
  Tensor Z(3, 6);
  Z.zeros();

  Dim i, j, k, l;
  Z[i][j].einsum((X[i][k][l] * Y[k][l][j]).sum(k, l));

  Z.print();
}
