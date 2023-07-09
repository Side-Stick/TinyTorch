include_directories(../CppLibraries)

add_executable(CppModuleTest
        main.cpp
        utils_test/HelloWorld_Test.cpp
        )

target_link_libraries(CppModuleTest PUBLIC TinyTorchCppSrc)
