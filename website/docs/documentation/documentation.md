# Documentation (使用文档)

## API


#### Dense Tensor

Construction methods

```c++
Tensor (int order, int const *len, int const *sym, World &wrld=get_universe(), char const *name=NULL, bool profile=0, tensor_int::algstrct const &sr=Ring< dtype >())
```

defines a tensor filled with zeros

```c++
Tensor (int order, int const *len, int const *sym, World &wrld, tensor_int::algstrct const &sr, char const *name=NULL, bool profile=0)
```

defines a nonsymmetric tensor filled with zeros

```c++
Tensor (int order, int const *len, World &wrld=get_universe(), tensor_int::algstrct const &sr=Ring< dtype >(), char const *name=NULL, bool profile=0)

Tensor (int order, bool is_sparse, int const *len, int const *sym, World &wrld=get_universe(), tensor_int::algstrct const &sr=Ring< dtype >(), char const *name=NULL, bool profile=0)
```


defines a nonsymmetric tensor filled with zeros on a specified algstrct   

```c++
Tensor (int order, bool is_sparse, int const *len, World &wrld=get_universe(), tensor_int::algstrct const &sr=Ring< dtype >(), char const *name=NULL, bool profile=0)

Tensor (Tensor< dtype > const &A)
```


copies a tensor, copying the data of A (same as above)   

```c++
cTensor (tensor const &A)
```


copies a tensor (setting data to zero or copying A)   

```c++
Tensor (bool copy, tensor const &A)
```

repacks the tensor other to a different symmetry 
(assumes existing data contains the symmetry and keeps only values with indices in increasing order) 

```c++
Tensor (tensor &A, int const *new_sym)
```

creates a zeroed out copy (data not copied) of a tensor in a different world  

```c++
Tensor (tensor const &A, World &wrld)
```


defines tensor filled with zeros on the default algstrct on a user-specified distributed layout  

```c++
Tensor (int order, int const *len, int const *sym, World &wrld, char const *idx, Idx_Partition const &prl, Idx_Partition const &blk=Idx_Partition(), char const *name=NULL, bool profile=0, tensor_int::algstrct const &sr=Ring< dtype >())

Tensor (int order, bool is_sparse, int const *len, int const *sym, World &wrld, char const *idx, Idx_Partition const &prl, Idx_Partition const &blk=Idx_Partition(), char const *name=NULL, bool profile=0, tensor_int::algstrct const &sr=Ring< dtype >())
```


* Linear Operations 

```c++
void  operator= (OpenTensor::Tensor< dtype > &values)

void  operator+= (OpenTensor::Tensor< dtype > &values)

void  operator-= (OpenTensor::Tensor< dtype > &values)

void  operator*= (OpenTensor::Tensor< dtype > &values)

void  operator= (OpenTensor::Sparse_Tensor<dtype> &values)

void  operator+= (OpenTensor::Sparse_Tensor<dtype > &values)

void  operator-= (OpenTensor::Sparse_Tensor<dtype> &values)

void  operator*= (OpenTensor::Sparse_Tensor<dtype> &values)


```



```c++
template<typename dtype = double>
void OpenTensor::Tensor< dtype >::sum 	( 	dtype  	alpha,
        OpenTensor::tensor &  	A,
        char const *  	idx_A,
        dtype  	beta,
        char const *  	idx_B 
    ) 		

Compute the expression:

B[idx_B] = beta*B[idx_B] + alpha*A[idx_A]

Parameters
    [in]	alpha	A scaling factor
    [in]	A	first operand tensor
    [in]	idx_A	indices of A in sum, e.g. "ij" -> A_{ij}
    [in]	beta	B scaling factor
    [in]	idx_B	indices of B (this tensor), e.g. "ij" -> B_{ij}
```



* Memory Operations 


```c++
template<typename dtype = double>
void OpenTensor::Tensor< dtype >::row_swap 	(         
        OpenTensor::tensor< dtype > & A,
        OpenTensor::tensor<int> & 	idx_A,
        OpenTensor::tensor<int> & 	idx_B,
    ) 		

swap the following rows: A[idx_A][:]  and A[idx_B][:]

template<typename dtype = double>
void OpenTensor::Tensor< dtype >::row_swap 	(         
        OpenTensor::tensor< dtype > & A,
        OpenTensor::tensor<int> & 	idx_A,
        OpenTensor::tensor<int> & 	idx_B,
        OpenTensor::tensor<int> & 	col_ind,
    ) 		

swap the following rows: A[idx_A][col_ind]  and A[idx_B][col_ind]
```



* Reduction methods


```c++
OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::norm1() 	

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::norm2() 	

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::infity() 	

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::min() 

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::sum() 	

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::argmin() 	

OpenTensor::Tensor< dtype > OpenTensor::Tensor< dtype >::asum() 	
```


A basic tensor algorithm is described as follows:

```c++
Tensor<> Jacobi(Tensor<> A , Tensor<> b , int n){
Tensor<> R(A);
R["ii"] = 0.0;
Tensor<> x (n), d(n), r(n);
Function<> inv([]( double & d ){ return 1./d; });
d["i"] = inv(A["ii"]);  
do {
    x["i"] = d["i"]*(b["i"]-R["ij"]*x["j"]);
    r["i"] = b["i"]-A ["ij"]*x["j"];  
} while ( r.norm2 () > 1. E -6);  // compute norm
return x;
}
```




#### Sparse Tensor

base constructor 

```c++
Sparse_Tensor ()
```

initialize a tensor which corresponds to a set of indices 

 	Sparse_Tensor (std::vector< int64_t > indices, Tensor< dtype > *parent)

initialize a tensor which corresponds to a set of indices  

 	Sparse_Tensor (int64_t n, int64_t *indices, Tensor< dtype > *parent)

set the sparse set of indices on the parent tensor to values forall(j) i = indices[j]; parent[i] = beta*parent[i] + alpha*values[j];   

    void write (dtype alpha, dtype *values, dtype beta)
    
    void operator= (OpenTensor::Sparse_Tensor< dtype > values)
    
    void operator+= (OpenTensor::Sparse_Tensor< dtype >values)
    
    void  operator-= (OpenTensor::Sparse_Tensor< dtype > values)
    
    void  operator= (OpenTensor::Tensor< dtype > values)
    
    void  operator+= (OpenTensor::Tensor< dtype > values)
    
    void  operator-= (OpenTensor::Tensor< dtype > values

read the sparse set of indices on the parent tensor to values forall(j) 

$i = indices[j];$ 

$values[j] = \alpha*parent[i] + \beta*values[j]; $ 

    void read (dtype alpha, dtype *values, dtype beta)
    
    operator std::vector< dtype > ()
    
    operator dtype * ()

A basic sparse tensor algorithm is described as follows:
Bellman-Ford computes the iterative scheme

$P_i^{(r)} = \min_j (A_{ij} + P_j^{(r-1)})$

starting from $r = 1$ and until convergence $P (r) = P (r−1)$, which will be reached by $r = |V|$, unless the graph contains negative-weight cycles. 

    template <typename t>
    bool Bellman_Ford(Matrix<t> A, Vector<t> P, int n){
    Vector<t> Q(P);
    int r = 0;
    OpenTensor::Tensor normP;
    OpenTensor::Tensor normQ;
    do { 
        if (r == n+1) return false;      // exit if we did not converge in n iterations
        else r++;
        Q["i"]  = P["i"];              // save old distances
        P["i"] += A["ij"]*P["j"];      // update distances 
        normQ = Q.norm1();
        normP = P.norm1();
    } while (normP < normQ); // continue so long as some distance got shorter
    return true;
    }



Kruscal Tensor
====================


Creating a one-dimensional ktensor

    template <typename dtype>
    KTensor Kruskal_Tensor(std::vector< dtype > values)

Kruskal tensor format via ktensor

    template <typename dtype>
    KTensor Kruskal_Tensor(std::vector< dtype > values, arg**)
    
    std::vector<double> A  = { 1,2,3};
    std::vector<double> B  = { 1,2,3};
    std::vector<double> C  = { 1,2,3,4,5,6};
    KTensor X = Kruskal_Tensor(A,B,C)  %<-- Create the ktensor.

Specifying weights in a ktensor

    template <typename dtype>
    void Kruskal_Tensor(std::vector< dtype > values, KTensor &)


Use full or tensor to convert a ktensor to a tensor

    template <typename dtype>
    KTensor Kruskal_Tensor(Tensor &)    

Use double to convert a ktensor to a multidimensional array

    template <typename dtype>
    void double(KTensor &, std::vector<dtype> &);    

Use ndims and size for the dimensions of a ktensor

    template <typename dtype>
    void dims(KTensor &)

Basic operations with a ktensor

    void operator= (std::vector< dtype > values)
    
    void operator+= (std::vector< dtype > values)
    
    void  operator-= (std::vector< dtype > values)
    
    void  operator= (dtype *values)
     
    void  operator+= (dtype *values)
    
    void  operator-= (dtype *values)

Use arrange to normalize the factors of a ktensor

    template <typename dtype>
    void normalize(KTensor &)

Displaying a ktensor


    template <typename dtype>
    void displaying(KTensor &)


* Reduction methods


```
OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::norm1() 	

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::norm2() 	

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::infity() 	

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::min() 

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::sum() 	

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::argmin() 	

OpenTensor::KTensor< dtype > OpenTensor::KTensor< dtype >::asum() 
```

