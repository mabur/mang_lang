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
