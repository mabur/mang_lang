cmake_minimum_required(VERSION 3.17)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(mang_lang_cpp
        main.cpp
        parser.h
        parser.cpp
        abstract_syntax_tree.h
        abstract_syntax_tree.cpp
        catch_amalgamated.hpp
        catch_amalgamated.cpp
)
