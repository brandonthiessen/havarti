#include "bench.h"

#include <iostream>
#include <numeric>
#include <vector>

void
run_bench(std::string name, std::function<BenchData()> f)
{
    BenchData data = f();

    BenchResult result = compute_bench_result(data);

    output_bench_result(result, name);
}

BenchResult
compute_bench_result(const BenchData& d)
{
    BenchResult r;

    int N = d.num_orders;
    r.num_orders = d.num_orders;

    auto pct_idx = [](int n, int p) -> size_t { return static_cast<size_t>(std::floor((p / 100.0) * (n + 1))); };

    if (d.start_ts != bench::TimePoint{}) {
        r.has_duration = true;
        r.duration = d.end_ts - d.start_ts;
    }

    if (!d.latencies_ns.empty()) {
        r.has_latency = true;

        std::vector<uint64_t> latencies = d.latencies_ns;
        std::sort(latencies.begin(), latencies.end());

        size_t p50_idx = pct_idx(N, 50);
        r.p50_latency_ns = latencies.at(p50_idx);

        size_t p99_idx = pct_idx(N, 99);
        r.p99_latency_ns = latencies.at(p99_idx);

        r.max_latency_ns = latencies.back();
    }

    if (!d.trades.empty()) {
        r.has_trade_stats = true;

        std::vector<size_t> trades_per_order;
        trades_per_order.reserve(N);

        for (const auto& v : d.trades) {
            trades_per_order.push_back(v.size());
        }

        r.num_trades = std::accumulate(trades_per_order.begin(), trades_per_order.end(), 0);

        std::sort(trades_per_order.begin(), trades_per_order.end());

        size_t p50_idx = pct_idx(N, 50);
        r.p50_trades_per_order = trades_per_order.at(p50_idx);

        size_t p99_idx = pct_idx(N, 99);
        r.p99_trades_per_order = trades_per_order.at(p99_idx);

        r.max_trades_per_order = trades_per_order.back();
    }

    return r;
}

void
output_bench_result(const BenchResult& r, std::string name)
{
    std::cout << "----- BENCHMARK RESULT: " << name << " -----\n";

    std::cout << "Orders: " << r.num_orders << "\n";
    if (r.num_trades > 0) {
        std::cout << "Trades: " << r.num_trades << "\n";
    }

    if (r.has_duration) {
        std::cout << "\n";

        std::cout << std::format(
                "Duration: {:.6f}s ({}ns)\n",
                bench::to_seconds(r.duration),
                r.duration.count());

        std::cout << std::format(
                "Throughput: {:.2f} orders/sec\n",
                r.num_orders / bench::to_seconds(r.duration));
    }

    if (r.has_latency) {
        std::cout << "\n--- Latency (ns) ---\n";
        std::cout << "p50: " << r.p50_latency_ns << "\n";
        std::cout << "p99: " << r.p99_latency_ns << "\n";
        std::cout << "Max: " << r.max_latency_ns << "\n";
    }

    if (r.has_trade_stats) {
        std::cout << "\n--- Trades per Order ---\n";
        std::cout << "p50 trades per order: " << r.p50_trades_per_order << "\n";
        std::cout << "p99 trades per order: " << r.p99_trades_per_order << "\n";
        std::cout << "Max trades per order: " << r.max_trades_per_order << "\n";
    }

    std::cout << std::endl;
}
