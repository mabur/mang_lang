cmake_minimum_required(VERSION 3.5)
project(manglang_project)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(library)

if(CMAKE_PROJECT_NAME STREQUAL manglang_project)
        add_subdirectory(app)
endif()
