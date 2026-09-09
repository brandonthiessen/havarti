cmake_minimum_required(VERSION 3.16)
project(havarti)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASENATIVE "-O3 -mcpu=native")

add_compile_options(
    $<$<CONFIG:Release>:-O3>
)

add_library(havarti_core
    src/orderbook.cpp
    src/async_trade_sink.cpp
    src/price_levels.cpp
)

target_include_directories(havarti_core PUBLIC include)

add_library(havarti_support
    support/mock_trade_sink.cpp
    support/order_generator.cpp
)

target_include_directories(havarti_support PUBLIC include support)

target_link_libraries(havarti_support PUBLIC havarti_core)

add_subdirectory(bench)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
endif()
