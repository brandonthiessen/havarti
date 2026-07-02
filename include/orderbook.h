#pragma once

#include <map>
#include <deque>

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

struct Trade {
    int buy_order_id;
    int sell_order_id;
    int64_t price;
    int quantity;
};

class OrderBook {
    public:
        std::vector<Trade> add_order(const Order& incoming);
    private:
        // Ordered by price (high-low)
        std::map<int64_t, std::deque<BookOrder>, std::greater<int64_t>> buys;

        // Ordered by price (low-high)
        std::map<int64_t, std::deque<BookOrder>, std::less<int64_t>> sells;
};
