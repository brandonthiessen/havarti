#include "async_trade_sink.h"
#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

namespace havarti::bench {

BenchData bench_throughput() {
    BenchData d;

    havarti::AsyncTradeSink sink(8192);
    havarti::OrderBook book(sink);
    havarti::support::OrderGenerator gen{42};

    size_t N = 100'000'000;
    d.num_orders = N;

    auto orders = gen.next_orders(N);

    d.start_ts = now();

    for (auto& o : orders) {
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
    havarti::bench::run_bench("bench_throughput", havarti::bench::bench_throughput);
}
