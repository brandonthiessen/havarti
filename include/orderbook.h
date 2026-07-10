#pragma once

#include "order.h"
#include "trade_sink.h"

#include <deque>
#include <map>

namespace havarti {

class OrderBook {
    public:
        OrderBook(TradeSink& sink);
        OrderBook(const OrderBook&) = delete;
        OrderBook& operator=(const OrderBook&) = delete;

        void add_order(const Order& incoming);
    private:
        // Ordered by price (high-low)
        std::map<int64_t, std::deque<BookOrder>, std::greater<int64_t>> buys;

        // Ordered by price (low-high)
        std::map<int64_t, std::deque<BookOrder>, std::less<int64_t>> sells;

        // Non-owning reference. TradeSink must outlive this OrderBook.
        TradeSink& sink_;
};

} // namespace havarti
