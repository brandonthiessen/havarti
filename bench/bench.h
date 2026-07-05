#pragma once

#include "trade.h"

#include <chrono>
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

namespace bench {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    inline TimePoint now() { return Clock::now(); }

    inline std::chrono::nanoseconds diff(TimePoint start, TimePoint end) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    inline double to_seconds(std::chrono::nanoseconds ns) {
        return std::chrono::duration<double>(ns).count();
    }
} // namespace bench

struct BenchResult {
    size_t num_orders = 0;
    size_t num_trades = 0;

    bool has_duration = false;
    std::chrono::nanoseconds duration{0};

    bool has_latency = false;
    uint64_t p50_latency_ns = 0;
    uint64_t p99_latency_ns = 0;
    uint64_t max_latency_ns = 0;

    bool has_trade_stats = false;
    uint64_t p50_trades_per_order = 0;
    uint64_t p99_trades_per_order = 0;
    uint64_t max_trades_per_order = 0;
};

struct BenchData {
    size_t num_orders = 0;

    std::vector<uint64_t> latencies_ns;
    std::vector<std::vector<Trade>> trades;

    bench::TimePoint start_ts{};
    bench::TimePoint end_ts{};
};

void run_bench(std::string name, std::function<BenchData()> f);
BenchResult compute_bench_result(const BenchData& d);
void output_bench_result(const BenchResult& r, std::string name);
