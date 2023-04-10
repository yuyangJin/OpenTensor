#ifndef EINSUM_H_
#define EINSUM_H_

#include <string>
// class Dim {};

class Tensor {
  double *tensor;

public:
  template <typename T, typename... Ts> Tensor(T dim1, Ts... dimn);
  void random();
  void zeros();
  void print();
  // Tensor &operator[](Dim &);
  // Tensor &operator*(Tensor &);
  // // template <typename T, typename... Ts> Tensor &sum(T dim1, Ts... dimn);
  // Tensor &sum(Dim &);
  // Tensor &sum(Dim &, Dim &);
  // Tensor &einsum(Tensor &);
  template <typename T, typename... Ts>
  Tensor &einsum(char *, T &arg1, Ts &...args);
  Tensor &operator[](std::string);
  Tensor &operator+(Tensor&);
  Tensor &operator-(Tensor&);
  Tensor &operator*(Tensor&);
  Tensor &operator/(Tensor&);
  
};

#endif // EINSUM_H_