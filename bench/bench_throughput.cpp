#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"
#include "trade_sink.h"

BenchData bench_throughput() {
    BenchData d;

    havarti::TradeSink sink(8192);
    havarti::OrderBook book(sink);
    havarti::OrderGenerator gen{42};

    size_t N = 10'000'000;
    d.num_orders = N;

    auto orders = gen.next_orders(N);

    d.start_ts = bench::now();

    for (auto& o : orders) {
        book.add_order(o);
    }

    d.end_ts = bench::now();

    return d;
}

int main() {
    run_bench("bench_throughput", bench_throughput);
}
