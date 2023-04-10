#include "einsum.h"

int main() {
  Tensor A(512, 128);
  A.random();
  Tensor B(128, 256);
  B.random();
  Tensor C(512, 256);
  C.zeros();

  // StencilKernel cal_phi;
  // cal_phi.domain(256, 256)
  // cal_phi.stencil
  // (0,0) - (-1,0) / (-1,0) - (-2, 0)

  B["2:-1, 2:-1"] = A["2:-1, 2:-1"] + A["1:-2, 2:-1"];


  C.print();
}
