cmake_minimum_required(VERSION 3.16)
project(havarti)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(orderbook
    src/orderbook.cpp
)

add_library(order_generator
    src/order_generator.cpp
)

target_include_directories(orderbook PUBLIC include)

target_include_directories(order_generator PUBLIC include)

add_executable(app
    src/main.cpp
)

target_link_libraries(app orderbook order_generator)
