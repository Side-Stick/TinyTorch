set(LIBRARY_OUTPUT_PATH ../../bin)

add_library(${PROJECT_NAME} SHARED
        utils/HelloWorld.cpp
        Tensor/TensorStorage.cpp
        )
