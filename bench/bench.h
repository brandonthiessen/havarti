#pragma once

#include "timing.h"
#include "trade.h"

#include <chrono>
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

namespace havarti::bench {

struct BenchResult {
    size_t num_orders = 0;
    size_t num_trades = 0;

    bool has_duration = false;
    std::chrono::nanoseconds duration{0};

    bool has_latency = false;
    uint64_t p50_latency_ns = 0;
    uint64_t p99_latency_ns = 0;
    uint64_t p999_latency_ns = 0;
    uint64_t p9999_latency_ns = 0;

    bool has_trade_stats = false;
    uint64_t p50_trades_per_order = 0;
    uint64_t p99_trades_per_order = 0;
    uint64_t max_trades_per_order = 0;
};

struct BenchData {
    size_t num_orders = 0;

    std::vector<uint64_t> latencies_ns;
    std::vector<std::vector<havarti::Trade>> trades;

    TimePoint start_ts{};
    TimePoint end_ts{};
};

void run_bench(std::string name, std::function<BenchData()> f);
BenchResult compute_bench_result(const BenchData& d);
void output_bench_result(const BenchResult& r, std::string name);

} // namespace havarti::bench
