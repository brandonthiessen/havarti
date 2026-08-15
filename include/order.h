#pragma once

#include <cstdint>

namespace havarti {

using Price = int64_t;
constexpr Price NO_PRICE = 0;

enum class Side {
    BUY, SELL
};

struct Order {
    int id;
    Side side;
    Price price;
    int quantity;
};

struct BookOrder {
    Order order;
    int remaining;
    BookOrder(const Order& o, int r)
        : order(o), remaining(r) {}
};

} // namespace havarti
