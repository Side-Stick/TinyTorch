cmake_minimum_required(VERSION 3.20)
project(TinyTorchCppSrc)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/hello_world.cpp)
