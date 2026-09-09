add_library(havarti_bench
    bench/bench.cpp
)

target_include_directories(havarti_bench PUBLIC bench include support)

target_link_libraries(havarti_bench PUBLIC havarti_core havarti_support)

add_executable(bench_throughput
    bench/bench_throughput.cpp
)

target_link_libraries(bench_throughput
    havarti_bench
)

add_executable(bench_latency
    bench/bench_latency.cpp
)

target_link_libraries(bench_latency
    havarti_bench
)

add_executable(bench_deep_book
    bench/bench_deep_book.cpp
)

target_link_libraries(bench_deep_book
    havarti_bench
)

add_executable(bench_one_sided_pressure
    bench/bench_one_sided_pressure.cpp
)

target_link_libraries(bench_one_sided_pressure
    havarti_bench
)

add_executable(bench_dense_price_levels
    bench/bench_dense_price_levels.cpp
)

target_link_libraries(bench_dense_price_levels
    havarti_bench
)
