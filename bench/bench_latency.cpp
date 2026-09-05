#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

#include <cstddef>
#include <cstdint>

BenchData bench_latency()
{
    BenchData d;

    constexpr size_t N = 10'000'000;
    constexpr size_t BATCH_SIZE = 100;

    havarti::TradeSink sink(8192);
    havarti::OrderBook book(sink);
    havarti::OrderGenerator gen{42};

    d.num_orders = N;

    auto orders = gen.next_orders(N);

    d.latencies_ns.reserve(N / BATCH_SIZE);

    for (size_t i = 0; i < N; i += BATCH_SIZE) {
        const size_t count =
            (i + BATCH_SIZE <= N)
                ? BATCH_SIZE
                : N - i;

        auto start = bench::now();

        for (size_t j = 0; j < count; ++j)
            book.add_order(orders[i + j]);

        auto end = bench::now();

        const auto batch_ns =
            bench::diff(start, end).count();

        const auto avg_ns =
            batch_ns / static_cast<uint64_t>(count);

        d.latencies_ns.push_back(avg_ns);
    }

    return d;
}

int main()
{
#ifdef __APPLE__
#include <pthread.h>
    pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
#endif
    run_bench("bench_latency", bench_latency);
}
