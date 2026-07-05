#include "bench.h"
#include "order_generator.h"
#include "orderbook.h"

BenchData bench_one_sided_pressure() {
    BenchData d;

    OrderBook book;

    // Generate orders biased 70:30 buys:sells
    OrderGenerator gen_buys{42, 9500, 9900, 1, 100};
    OrderGenerator gen_sells{43, 10100, 10500, 1, 100};

    size_t num_buys = 7'000'000;
    size_t num_sells = 3'000'000;
    d.num_orders = num_buys + num_sells;

    auto buys = gen_buys.next_orders(num_buys, Side::BUY);
    auto sells = gen_sells.next_orders(num_sells, Side::SELL);

    // Interleave: 70% from buy queue, 30% from sell queue
    std::vector<Order> orders;
    orders.reserve(num_buys + num_sells);
    int i = 0, j = 0, k = 0;
    while (i < num_buys || j < num_sells) {
        if ((++k % 10) < 7 && i < num_buys) {
            orders.push_back(buys[i++]);
        } else if (j < num_sells) {
            orders.push_back(sells[j++]);
        }
    }

    d.start_ts = bench::now();

    for (auto& o : orders) {
        book.add_order(o);
    }

    d.end_ts = bench::now();

    return d;
}

int main() {
    run_bench("bench_one_sided_pressure", bench_one_sided_pressure);
}
