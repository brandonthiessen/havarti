#include "bench.h"
#include "order.h"
#include "order_generator.h"
#include "orderbook.h"
#include "trade_sink.h"

BenchData
bench_dense_price_levels()
{
    BenchData d;

    havarti::TradeSink sink(8192);
    havarti::OrderBook book(sink);

    constexpr int DENSE_MIN = 7952;
    constexpr int DENSE_MAX = 12047;

    // Every generated order is guaranteed to lie inside the dense price-level range.
    havarti::OrderGenerator gen{
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
    book.add_order({-3, havarti::Side::BUY, 10000, 1});
    book.add_order({-2, havarti::Side::SELL, 10000, 2});
    book.add_order({-1, havarti::Side::BUY, 10000, 1});

    d.start_ts = bench::now();

    for (auto& o : orders) {
        book.add_order(o);
    }

    d.end_ts = bench::now();

    return d;
}

int main()
{
    run_bench("bench_dense_price_levels", bench_dense_price_levels);
}
