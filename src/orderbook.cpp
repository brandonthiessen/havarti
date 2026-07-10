#include "orderbook.h"

namespace havarti {

OrderBook::OrderBook(TradeSink& sink):
    sink_(sink)
{}

void
OrderBook::add_order(const Order& incoming)
{
    int remaining_qty = incoming.quantity;

    if (incoming.side == Side::BUY) {

        // Match incoming buy orders against lowest available sell prices
        for (auto it = sells.begin(); it != sells.end() && remaining_qty > 0; ) {
            auto& [resting_price, resting_queue] = *it;

            // Stop if asks are no longer matchable
            if (resting_price > incoming.price) {
                break;
            }

            // Consume FIFO orders at this price level
            while (remaining_qty > 0 && !resting_queue.empty()) {
                auto& resting = resting_queue.front();

                int filled_qty = std::min(remaining_qty, resting.remaining);
                if (!sink_.submit(Trade(incoming.id, resting.order.id, resting_price, filled_qty))) {
                    // TODO: handle failure
                }

                remaining_qty -= filled_qty;
                resting.remaining -= filled_qty;

                if (resting.remaining == 0) {
                    resting_queue.pop_front();
                }
            }

            // Remove empty price levels
            if (resting_queue.empty()) {
                it = sells.erase(it);
                continue;
            } else {
                ++it;
            }
        }

        if (remaining_qty > 0) {
            // Use incoming price as resting price
            buys[incoming.price].push_back(BookOrder(incoming, remaining_qty));
        }
    } else { // incoming.side == Side::SELL

        // Match incoming sell orders against highest available buy prices
        for (auto it = buys.begin(); it != buys.end() && remaining_qty > 0; ) {
            auto& [resting_price, resting_queue] = *it;

            // Stop if bids are no longer matchable
            if (resting_price < incoming.price) {
                break;
            }

            // Consume FIFO orders at this price level
            while (remaining_qty > 0 && !resting_queue.empty()) {
                auto& resting = resting_queue.front();

                int filled_qty = std::min(remaining_qty, resting.remaining);
                if (!sink_.submit(Trade(resting.order.id, incoming.id, resting_price, filled_qty))) {
                    // TODO: handle failure
                }
                remaining_qty -= filled_qty;
                resting.remaining -= filled_qty;

                if (resting.remaining == 0) {
                    resting_queue.pop_front();
                }
            }

            // Remove empty price levels
            if (resting_queue.empty()) {
                it = buys.erase(it);
                continue;
            } else {
                ++it;
            }
        }

        if (remaining_qty > 0) {
            // Use incoming price as resting price
            sells[incoming.price].push_back(BookOrder(incoming, remaining_qty));
        }
    }
}

} // namespace havarti
