cmake_minimum_required(VERSION 3.17)
project(mang_lang_cpp)

set(CMAKE_CXX_STANDARD 20)

add_library(shared
        built_in_functions/built_in_functions.cpp
        expressions/Character.cpp
        expressions/Conditional.cpp
        expressions/Dictionary.cpp
        expressions/DictionaryElement.cpp
        expressions/Expression.cpp
        expressions/Function.cpp
        expressions/FunctionBuiltIn.cpp
        expressions/FunctionDictionary.cpp
        expressions/FunctionList.cpp
        expressions/List.cpp
        expressions/LookupChild.cpp
        expressions/LookupFunction.cpp
        expressions/LookupSymbol.cpp
        expressions/Name.cpp
        expressions/Number.cpp
        expressions/String.cpp
        operations/apply.cpp
        operations/boolean.cpp
        operations/character.cpp
        operations/empty.cpp
        operations/evaluate.cpp
        operations/is_equal.cpp
        operations/jump.cpp
        operations/list.cpp
        operations/lookup.cpp
        operations/mutate.cpp
        operations/number.cpp
        operations/parse.cpp
        operations/prepend.cpp
        operations/serialize.cpp
        operations/starts_with.cpp
        factory.cpp
        mang_lang.cpp
        parsing.cpp
        )

add_executable(tests tests.cpp)
target_link_libraries(tests shared)
target_compile_options(shared PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_options(tests PRIVATE -Wall -Wextra -pedantic -Werror)
