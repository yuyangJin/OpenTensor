#ifndef OPENTENSOR_H_
#define OPENTENSOR_H_

class Dim {};

class Tensor {
  double *tensor;

public:
  void ot_random(int);
  void ot_zeros(int);
  void ot_print();
  Tensor &operator[](Dim &);
  Tensor &operator*(Tensor &);
  Tensor &sum(Dim &);
  Tensor &einsum(Tensor &);
};

#endif // OPENTENSOR_H_