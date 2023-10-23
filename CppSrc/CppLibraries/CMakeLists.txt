set(LIBRARY_OUTPUT_PATH ../../bin)

include_directories(../CppLibraries)

add_library(${PROJECT_NAME} SHARED
        utils/HelloWorld.cpp
        Tensor/TensorStorage.cpp
        Tensor/intrusive_ptr.cpp
        utils/Exception.cpp
)
