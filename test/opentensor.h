#ifndef OPENTENSOR_H_
#define OPENTENSOR_H_

class Dim {};

class Tensor {
  double *tensor;

public:
  template<typename T, typename... Ts>
  Tensor (T dim1, Ts... dimn);
  void random();
  void zeros();
  void print();
  Tensor &operator[](Dim &);
  Tensor &operator*(Tensor &);
  Tensor &sum(Dim &);
  Tensor &einsum(Tensor &);
};

#endif // OPENTENSOR_H_