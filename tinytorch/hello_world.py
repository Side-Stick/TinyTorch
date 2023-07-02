#! -*- coding:utf-8 -*-
from ctypes import CDLL, c_char_p

from .config import CPP_DLL_PATH


def hello_tinytorch():
    print("Hello TinyTorch! From Python script.")
    libc = CDLL(CPP_DLL_PATH)

    my_name = c_char_p(b"TinyTorch_Cpp")
    libc.hello_world.restype = c_char_p
    # Set the return type of C++ lib, default is C int.
    message_from_cpp = libc.hello_world(my_name)
    print(message_from_cpp.decode())        # bytes to str
