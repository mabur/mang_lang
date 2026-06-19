cmake_minimum_required(VERSION 3.5)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(library)

if(CMAKE_PROJECT_NAME STREQUAL mang_lang_cpp)
        add_executable(manglang interpreter.cpp)
        add_executable(tests tests.cpp)

        target_link_libraries(manglang mang_lang)
        target_link_libraries(tests mang_lang)

        if (NOT MSVC)
                target_compile_options(manglang PRIVATE -Wall -pedantic -Werror)
                target_compile_options(tests PRIVATE -Wall -pedantic -Werror)
        endif()

        target_link_options(manglang PRIVATE -static)
        target_link_options(tests PRIVATE -static)
endif()
