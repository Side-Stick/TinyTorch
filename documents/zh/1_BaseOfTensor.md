# Tensor的理论和数据结构基础

## 简要理解Tensor

Tensor，张量，原本是一个力学中的概念，在这里我们可以简单地理解为多维数组，这个多维数组的轴的数量称为阶（rank），与维度（dimension）区分；例如向量就是1阶张量，矩阵就是2阶张量。Tensor在现代机器学习中具有基础性的作用，现阶段几乎所有的机器学习底层结构均由张量运算支撑。

## Tensor的数据结构与代码实现

### 我们希望class Tensor做到什么？

**属性：**
（1）一块存储数据的内存，以及指向该内存的指针、引用计数器、内存偏移量等。不同的数据存储形式还应共享相同的接口，如PyTorch中的DataPtr类（`pytorch/c10/core/Allocator.h`和`pytorch/c10/util/UniqueVoidPtr.h`）。
（2）shape、dtype等数据解释方式。
（3）存储设备（device，由此引申，还需要不同的allocator等）、数据组织形式layout（如通常的稠密矩阵stride tensor、稀疏矩阵sparse tensor等）。
（4）自动微分（autograd，也是PyTorch Tensor与Numpy Array最具代表性的区别）。
（5）在PyTorch中，为了调用针对性优化的算法，还有DispatchKey。

**特性：**
（1）高效性：矩阵运算是机器学习算法中的底层支柱，Tensor相关代码的执行效率至关重要。例如，我们会将CPU Tensor存储在RAM（栈）中而非ROM（堆）中，PyTorch中（`pytorch/c10/core/impl/alloc_cpu.cpp`）就采用了【GCC】posix_memalign()和【MSVC】_aligned_malloc()来分配栈空间。
（2）拓展性：对于不同的硬件设备（比如CPU和CUDA），可分别实现不同的优化算法；它们应共享相同的接口，且在新设备出现时，应能进行拓展。但是，为了执行效率，我们不希望在一些非常常用的函数（如size of等）中使用虚函数，这样会严重降低执行效率。

### 在Pytorch中的实现

PyTorch版本更新比较快，这里针对[PyTorch 2.0.1 Release](https://github.com/pytorch/pytorch/releases/tag/v2.0.1)的源代码进行分析。

![PyTorch中Tensor的数据结构](../pictures/1.1-PytorchTensorUML.jpg)

PyTorch 2.0.1中，Tensor在C++代码结构中主要分为4层，顶层为TensorBase，包含自动微分等接口；实现（implement）层为TensorImpl，储存了如size、dtype等元数据；第三层为Storage和StorageImpl，前者为后者的接口类，主要负责管理底层数据的空间分配等；最底层为一些数据分配和指针类，用于在不同的软硬件平台上实现高效的内存管理。但是在issue([https://github.com/pytorch/pytorch/issues/14797](https://github.com/pytorch/pytorch/issues/14797))和StorageImpl.h的代码注释中，pytorch团队提到因为接口过于老旧等原因，希望废弃Storage类，但因为工程量较大，一直未能实现（该issue为2018年提出，但截至2023年仍为实现）。

例如当用户在python代码中调用`torch.Tensor()`创建张量时，通过逐层接口调用，底层的某个Allocator通过`allocate()`方法分配了一块内存（或显存）空间，由StorageImpl类进行调度，实际指针在DataPtr（UniqueVoidPtr）类中保存。

### 本项目中的实现

本项目中，将同样采取底层数据与上层应用接口分离的方案，Tensor数据结构将分为2层，分别为Tensor和TensorStorage。
