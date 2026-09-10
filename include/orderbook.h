#pragma once

#include "order.h"
#include "price_levels.h"
#include "trade_sink.h"

namespace havarti {

class OrderBook {
    public:
        OrderBook(TradeSink& sink);
        OrderBook(const OrderBook&) = delete;
        OrderBook& operator=(const OrderBook&) = delete;

        void add_order(const Order& incoming);
        bool cancel_order(const OrderId id);

        PriceLevels<Side::BUY>& get_buys() { return buys_; }
        PriceLevels<Side::SELL>& get_sells() { return sells_; }
    private:
        PriceLevels<Side::BUY> buys_; // Ordered by price (high-low)
        PriceLevels<Side::SELL> sells_; // Ordered by price (low-high)

        // Non-owning reference. TradeSink must outlive this OrderBook.
        TradeSink& sink_;
};

} // namespace havarti
