#pragma once

#include "order.h"

static constexpr int64_t MID_PRICE = 1000; // 10.00

#include <random>

class OrderGenerator {
    public:
        OrderGenerator(int seed);
        OrderGenerator(int seed, int64_t price_low, int64_t price_high, int qty_low, int qty_high);
        Order next_order();
        std::vector<Order> next_orders(int n);
    private:
        std::mt19937 rng;
        std::uniform_int_distribution<int> side_dist;
        std::uniform_int_distribution<int64_t> price_dist;
        std::uniform_int_distribution<int> qty_dist;

        int next_id = 1;
};
