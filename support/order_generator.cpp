#include "order_generator.h"

#include <cmath>

namespace havarti::support {

BoundedNormalPrice::BoundedNormalPrice(double mean, double stddev, Price min, Price max)
    : dist_(mean, stddev),
      min_(min),
      max_(max)
{}

Price
BoundedNormalPrice::operator()(std::mt19937& rng)
{
    while (true) {
        const double price = dist_(rng);

        if (price >= min_ && price <= max_)
            return static_cast<Price>(std::round(price));
    }
}

OrderGenerator::OrderGenerator(int seed)
    : OrderGenerator(
        seed,
        [](std::mt19937& rng) {
            return std::uniform_int_distribution<Price>{
                MID_PRICE - 50, MID_PRICE + 50
            }(rng);
        },
        [](std::mt19937& rng) {
            return std::uniform_int_distribution<Quantity>{
                1, 100
            }(rng);
        })
{}

OrderGenerator::OrderGenerator(int seed, PriceDistribution price_dist, QuantityDistribution qty_dist)
    : rng(seed),
      side_dist(0, 1),
      price_dist(std::move(price_dist)),
      qty_dist(std::move(qty_dist))
{}

OrderGenerator::OrderGenerator( int seed, Price price_low, Price price_high, Quantity qty_low, Quantity qty_high)
    : OrderGenerator(
        seed,
        [price_low, price_high](std::mt19937& rng) {
            return std::uniform_int_distribution<Price>{
                price_low, price_high
            }(rng);
        },
        [qty_low, qty_high](std::mt19937& rng) {
            return std::uniform_int_distribution<Quantity>{
                qty_low, qty_high
            }(rng);
        })
{}

Order
OrderGenerator::next_order()
{
    return Order{
        next_id++,
        price_dist(rng),
        qty_dist(rng),
        side_dist(rng) == 0 ? Side::BUY : Side::SELL
    };
}

Order
OrderGenerator::next_order(Side side)
{
    return Order{
        next_id++,
        price_dist(rng),
        qty_dist(rng),
        side
    };
}

std::vector<Order>
OrderGenerator::next_orders(int n)
{
    std::vector<Order> orders;
    orders.reserve(n);

    for (int i = 0; i < n; i++) {
        orders.push_back(next_order());
    }

    return orders;
}

std::vector<Order>
OrderGenerator::next_orders(int n, Side side)
{
    std::vector<Order> orders;
    orders.reserve(n);

    for (int i = 0; i < n; i++) {
        orders.push_back(next_order(side));
    }

    return orders;
}

} // namespace havarti::support
