#pragma once

#include "order.h"

#include <random>

namespace havarti::support {

static constexpr int64_t MID_PRICE = 1000; // 10.00

class OrderGenerator {
    public:
        OrderGenerator(int seed);
        OrderGenerator(int seed, Price price_low, Price price_high, Quantity qty_low, Quantity qty_high);
        Order next_order();
        Order next_order(Side side);
        std::vector<Order> next_orders(int n);
        std::vector<Order> next_orders(int n, Side side);
    private:
        std::mt19937 rng;
        std::uniform_int_distribution<int> side_dist;
        std::uniform_int_distribution<Price> price_dist;
        std::uniform_int_distribution<Quantity> qty_dist;

        OrderId next_id = 1;
};

} // namespace havarti::support
