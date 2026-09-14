#pragma once

#include "order.h"

#include <functional>
#include <random>
#include <vector>

namespace havarti::support {

static constexpr Price MID_PRICE = 1000;

class BoundedNormalPrice {
    public:
        BoundedNormalPrice(double mean, double stddev, Price min, Price max);

        Price operator()(std::mt19937& rng);

    private:
        std::normal_distribution<double> dist_;
        Price min_;
        Price max_;
};

class OrderGenerator {
    public:
        using PriceDistribution = std::function<Price(std::mt19937&)>;
        using QuantityDistribution = std::function<Quantity(std::mt19937&)>;

        OrderGenerator(int seed);
        OrderGenerator(int seed, PriceDistribution price_dist, QuantityDistribution qty_dist);
        OrderGenerator(int seed, Price price_low, Price price_high, Quantity qty_low, Quantity qty_high);

        Order next_order();
        Order next_order(Side side);

        std::vector<Order> next_orders(int n);
        std::vector<Order> next_orders(int n, Side side);

    private:
        std::mt19937 rng;
        std::uniform_int_distribution<int> side_dist;

        PriceDistribution price_dist;
        QuantityDistribution qty_dist;

        OrderId next_id = 1;
};

} // namespace havarti::support
