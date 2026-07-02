#pragma once

#include <cstdint>

enum class Side {
    BUY, SELL
};

struct Order {
    int id;
    Side side;
    int64_t price;
    int quantity;
};

struct BookOrder {
    Order order;
    int remaining;
    BookOrder(const Order& o, int r)
        : order(o), remaining(r) {}
};
