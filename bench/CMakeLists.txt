add_library(havarti_bench
    bench.cpp
)

target_include_directories(havarti_bench PUBLIC bench include support)

target_link_libraries(havarti_bench PUBLIC havarti_core havarti_support)

function(add_havarti_benchmark name)
    add_executable(bench_${name}
        bench_${name}.cpp
    )

    target_link_libraries(bench_${name}
        havarti_bench
    )
endfunction()

add_havarti_benchmark(throughput)
add_havarti_benchmark(latency)
add_havarti_benchmark(deep_book)
add_havarti_benchmark(one_sided_pressure)
add_havarti_benchmark(dense_price_levels)
