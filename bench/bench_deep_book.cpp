#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

BenchData bench_deep_book() {
    BenchData d;

    OrderBook book;
    OrderGenerator gen{42};

    // Pre-populate with 100k buy orders
    size_t warmup = 100'000;
    auto warmup_orders = gen.next_orders(warmup);
    for (auto& o : warmup_orders) {
        book.add_order(o);
    }

    // Now send 1M sell orders to hit that deep book
    size_t N = 1'000'000;
    auto aggressive_orders = gen.next_orders(N, Side::SELL);
    d.num_orders = N;

    d.start_ts = bench::now();

    for (auto& o : aggressive_orders) {
        book.add_order(o);
    }

    d.end_ts = bench::now();

    return d;
}

int main() {
    run_bench("bench_deep_book", bench_deep_book);
}
