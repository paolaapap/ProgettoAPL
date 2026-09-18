cmake_minimum_required(VERSION 3.16)
project(APL_Backend CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/memory_tracer.cpp
    src/benchmark.cpp
    src/algorithms/sorting.cpp
    src/algorithms/search.cpp
    src/algorithms/graphs.cpp
)

add_executable(backend ${SOURCES})

# per portabilità su windows anche senza mingw
if(MINGW)
    target_link_options(backend PRIVATE -static)
endif()
