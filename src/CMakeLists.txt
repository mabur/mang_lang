cmake_minimum_required(VERSION 3.5)
project(manglang_project)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(library)

if(CMAKE_PROJECT_NAME STREQUAL manglang_project)
        add_executable(manglang interpreter.cpp)
        add_executable(manglang_tests tests.cpp)

        target_link_libraries(manglang manglang_lib)
        target_link_libraries(manglang_tests manglang_lib)

        if (NOT MSVC)
                target_compile_options(manglang PRIVATE -Wall -pedantic -Werror)
                target_compile_options(manglang_tests PRIVATE -Wall -pedantic -Werror)
        endif()

        target_link_options(manglang PRIVATE -static)
        target_link_options(manglang_tests PRIVATE -static)
endif()
