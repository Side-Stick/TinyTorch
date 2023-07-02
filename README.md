# TinyTorch
这是一个简单的机器学习框架，是笔者的“小黄鸭调试”草稿。An [English Version](./README.en.md) is (will be) provided as well.

项目将主要分为文档和代码两部分：文档主要为markdown格式，代码则分为C/C++等底层代码、python应用层框架代码、python测试代码等，未来有可能加入CUDA等硬件支持，但短期内仅考虑支持CPU。

## 文档目录
- [关于环境配置](#关于环境配置)
- [API](documents/zh/API.md)
- 0 预备知识
    - [0.1 通过ctype将C/C++代码动态库与python连接](documents/zh/0_StructureOfProj.md)
    - 

## 关于环境配置

对C/C++代码，遵循C++ 17规范，代码风格尽量遵循Google开源代码风格指南，使用gcc(g++)11编译器。

对python代码，使用python 3.9解释器，尽量不依赖第三方库。