cmake_minimum_required(VERSION 3.17)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(mang_lang_cpp
        catch_amalgamated.hpp
        catch_amalgamated.cpp
        main.cpp
        mang_lang.cpp
        parse_utils.cpp
        expressions/Conditional.cpp
        expressions/Dictionary.cpp
        expressions/Expression.cpp
        expressions/List.cpp
        expressions/LookupChild.cpp
        expressions/LookupSymbol.cpp
        expressions/Number.cpp
        expressions/String.cpp
        )
