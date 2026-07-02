#pragma once

#include "order.h"
#include "trade.h"

#include <deque>
#include <map>
#include <vector>

class OrderBook {
    public:
        std::vector<Trade> add_order(const Order& incoming);
    private:
        // Ordered by price (high-low)
        std::map<int64_t, std::deque<BookOrder>, std::greater<int64_t>> buys;

        // Ordered by price (low-high)
        std::map<int64_t, std::deque<BookOrder>, std::less<int64_t>> sells;
};
