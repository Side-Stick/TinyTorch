from ctypes import *


def hello_world():
    print("Hello TinyTorch! From Python script.")
    libc = CDLL('../CppSrc/cmake-build-debug/libTinyTorchCppSrc.dylib')

    my_name = c_char_p(b"TinyTorch_Cpp")
    libc.hello_world.restype = c_char_p     # 设置Cpp库返回类型，否则默认为C int。
    message_from_cpp = libc.hello_world(my_name)
    print(message_from_cpp.decode())        # bytes转str


if '__main__' == __name__:
    hello_world()
