#include "async_trade_sink.h"
#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

namespace havarti::bench {

BenchData bench_deep_book() {
    BenchData d;

    havarti::AsyncTradeSink sink(8192);
    havarti::OrderBook book(sink);
    havarti::support::OrderGenerator gen{42};

    // Pre-populate with 100k buy orders
    size_t warmup = 100'000;
    auto warmup_orders = gen.next_orders(warmup);
    for (auto& o : warmup_orders) {
        book.add_order(o);
    }

    // Now send 1M sell orders to hit that deep book
    size_t N = 1'000'000;
    auto aggressive_orders = gen.next_orders(N, havarti::Side::SELL);
    d.num_orders = N;

    d.start_ts = now();

    for (auto& o : aggressive_orders) {
        book.add_order(o);
    }

    d.end_ts = now();

    return d;
}

} // namespace havarti::bench

int main() {
#ifdef __APPLE__
#include <pthread.h>
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
#endif
    havarti::bench::run_bench("bench_deep_book", havarti::bench::bench_deep_book);
}
