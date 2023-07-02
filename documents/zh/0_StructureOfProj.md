# 项目结构

## 主要文件目录整理思路

对python部分，为方便测试，将pycharm项目的内容根设置为根目录（TinyTorch），但.idea文件夹在PythonProjTest目录下；tinytorch目录下为项目的python代码，PythonProjTest目录下为项目整体测试所使用的测试代码。这样的好处是，pycharm会自动将根目录加入sys.path，在PythonProjTest目录下的脚本中调用tinytorch包时，可以免去相对路径导入的麻烦。

对C++部分，核心代码集中于CppSrc目录下，在此目录下有一CLion项目（含.idea文件夹、cmake-build-debug和cmake-build-release等目录），编译产生的动态库位于CppSrc/bin目录下。

项目的文件树可抽象为：
```
TinyTorch
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
|   |-- .idea
|-- py_envs
|-- documents
```

## ctypes库与两种语言的链接

