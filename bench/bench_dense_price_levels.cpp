#include "async_trade_sink.h"
#include "bench.h"
#include "order.h"
#include "order_generator.h"
#include "orderbook.h"

namespace havarti::bench {

BenchData
bench_dense_price_levels()
{
    BenchData d;

    havarti::AsyncTradeSink sink(8192);
    havarti::OrderBook book(sink);

    constexpr int DENSE_MIN = 7952;
    constexpr int DENSE_MAX = 12047;

    // Every generated order is guaranteed to lie inside the dense price-level range.
    havarti::support::OrderGenerator gen{
        42,
        DENSE_MIN,
        DENSE_MAX,
        1,
        100
    };

    size_t N = 10'000'000;
    d.num_orders = N;

    auto orders = gen.next_orders(N);

    // Insert buy and sell orders to center the dense window at $100 on both sides.
    book.add_order({INT32_MAX - 1, 10000, 1, havarti::Side::BUY});
    book.add_order({INT32_MAX - 2, 10000, 2, havarti::Side::SELL});
    book.add_order({INT32_MAX - 3, 10000, 1, havarti::Side::BUY});

    d.start_ts = bench::now();

    for (auto& o : orders) {
        book.add_order(o);
    }

    d.end_ts = bench::now();

    return d;
}

} // namespace havarti::bench

int main()
{
#ifdef __APPLE__
#include <pthread.h>
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
#endif
    havarti::bench::run_bench("bench_dense_price_levels", havarti::bench::bench_dense_price_levels);
}
