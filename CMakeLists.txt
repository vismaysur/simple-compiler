cmake_minimum_required(VERSION 3.28)

project(hydrogen_clone)

set(CMAKE_CXX_STANDARD 20)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

add_executable(hydro src/main.cpp
        src/tokenization.hpp
        src/parser.hpp
        src/generation.hpp
)