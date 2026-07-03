#include "order_generator.h"

OrderGenerator::OrderGenerator(int seed)
    : rng(seed), side_dist(0, 1), price_dist(MID_PRICE - 50, MID_PRICE + 50), qty_dist(1, 100), next_id(1)
{}

OrderGenerator::OrderGenerator(int seed, int64_t price_low, int64_t price_high, int qty_low, int qty_high)
    : rng(seed), side_dist(0, 1), price_dist(price_low, price_high), qty_dist(qty_low, qty_high), next_id(1)
{}

Order
OrderGenerator::next_order()
{
    return Order{next_id++, side_dist(rng) == 0 ? Side::BUY : Side::SELL, price_dist(rng), qty_dist(rng)};
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
