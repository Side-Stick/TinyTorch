# 项目基本结构

## 主要文件目录整理思路

对python部分，为方便测试，将pycharm项目的内容根设置为根目录（TinyTorch）；tinytorch目录下为项目的python代码，PythonProjTest目录下为项目整体测试所使用的测试代码。这样的好处是，pycharm会自动将根目录加入sys.path，在PythonProjTest目录下的脚本中调用tinytorch包时，可以免去相对路径导入的麻烦。

对C++部分，核心代码集中于CppSrc目录下，在此目录下有一CLion项目（含.idea文件夹、cmake-build-debug和cmake-build-release等目录），编译产生的动态库位于CppSrc/bin目录下。

项目的文件树可抽象为：
```
TinyTorch
|-- .idea
|-- README.md
|-- README.en.md
|-- CppSrc
|   |-- (code files)
|   |-- bin
|   |-- |-- xxx.dylib
|   |-- cmake-build-xxx
|   |-- .idea
|-- tinytorch
|   |-- (code files)
|-- PythonProjTest
|   |-- (code files)
|-- py_envs
|-- documents
```

## 使用ctypes库链接C++和Python

使用python标准库ctypes中的CDLL对象实现C++动态库的加载。这种方法的好处是并不局限于CPython，对不同版本python编译器的兼容性更好。

使用config.py文件配置C++动态库的路径配置，在macOS平台上拓展名为dylib（在Windows平台上为dll，在Linux平台上为so）；加载后，可直接使用C++动态库中的函数名调用CDLL对象的属性，注意可能需要提前指定动态库函数的返回类型（restype）。此外，在C++项目中，必须使用`extern "C"`来修饰头文件中函数声明。