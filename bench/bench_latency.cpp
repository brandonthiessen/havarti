#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

BenchData bench_latency() {
    BenchData d;

    havarti::TradeSink sink(8192);
    havarti::OrderBook book(sink);
    havarti::OrderGenerator gen{42};

    size_t N = 1'000'000;
    d.num_orders = N;

    auto orders = gen.next_orders(N);

    for (auto& o : orders) {
        auto start = bench::now();

        // TODO: collect Trade metrics without storing each trade
        book.add_order(o);
        auto end = bench::now();

        d.latencies_ns.push_back(bench::diff(start, end).count());
    }

    return d;
}

int main() {
    run_bench("bench_latency", bench_latency);
}
