# Tensor的理论和数据结构基础

## 简要理解Tensor

Tensor，张量，原本是一个力学中的概念，在这里我们可以简单地理解为多维数组，这个多维数组的轴的数量称为阶（rank），与维度（dimension）区分；例如向量就是1阶张量，矩阵就是2阶张量。Tensor在现代机器学习中具有基础性的作用，现阶段几乎所有的机器学习底层结构均由张量运算支撑。

## Tensor的数据结构与代码实现

在pytorch 2.0.1中，Tensor在C++代码中的结构分为4层，从顶层开始分别为Tensor，TensorImpl，Storage，StorageImpl，其中Impl为implementation的缩写，用于分离外部可见类和实现细节。但是在issue([https://github.com/pytorch/pytorch/issues/14797](https://github.com/pytorch/pytorch/issues/14797))和StorageImpl.h的代码注释中，pytorch团队提到因为接口过于老旧等原因，希望废弃Storage类，但因为工程量较大，一直未能实现（该issue为2018年提出，但截至2023年仍为实现）。

本项目中，将同样采取底层数据与上层应用接口分离的方案，Tensor数据结构将分为2层，分别为Tensor和TensorStorage。