cmake_minimum_required(VERSION 3.16)
project(havarti)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(havarti_core
    src/orderbook.cpp
    src/order_generator.cpp
)

target_include_directories(havarti_core PUBLIC include)

add_library(havarti_bench
    bench/bench.cpp
)

target_include_directories(havarti_bench PUBLIC bench include)

target_link_libraries(havarti_bench PUBLIC havarti_core)

add_executable(app
    src/main.cpp
)

target_link_libraries(app
    havarti_core
)
