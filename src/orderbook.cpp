#include "orderbook.h"
#include "order.h"

namespace havarti {

OrderBook::OrderBook(TradeSink& sink):
    sink_(sink)
{}

void
OrderBook::add_order(const Order& incoming)
{
    int remaining_qty = incoming.quantity;

    if (incoming.side == Side::BUY) {

        Price resting_price = NO_PRICE;

        // Match incoming buy orders against lowest available sell prices
        while (remaining_qty > 0 &&
               (resting_price = sells_.best_price()) != 0 &&
               resting_price <= incoming.price) {

            // Consume FIFO orders at this price level
            while (remaining_qty > 0 && !sells_.empty(resting_price)) {
                BookOrder& resting = sells_.front(resting_price);

                int filled_qty = std::min(remaining_qty, resting.remaining);
                if (!sink_.submit(Trade(incoming.id, resting.order.id, resting_price, filled_qty))) {
                    // TODO: handle failure
                }

                remaining_qty -= filled_qty;
                resting.remaining -= filled_qty;

                if (resting.remaining == 0) {
                    sells_.pop_front(resting_price);
                }
            }
        }

        if (remaining_qty > 0) {
            // Use incoming price as resting price
            buys_.push_back(incoming.price, BookOrder(incoming, remaining_qty));
        }

    } else { // incoming.side == Side::SELL

        Price resting_price = NO_PRICE;

        // Match incoming sell orders against highest available buy prices
        while (remaining_qty > 0 &&
               (resting_price = buys_.best_price()) != NO_PRICE &&
               resting_price >= incoming.price) {

            // Consume FIFO orders at this price level
            while (remaining_qty > 0 && !buys_.empty(resting_price)) {
                BookOrder& resting = buys_.front(resting_price);

                int filled_qty = std::min(remaining_qty, resting.remaining);

                if (!sink_.submit(Trade(resting.order.id, incoming.id, resting_price, filled_qty))) {
                    // TODO: handle failure
                }

                remaining_qty -= filled_qty;
                resting.remaining -= filled_qty;

                if (resting.remaining == 0) {
                    buys_.pop_front(resting_price);
                }
            }
        }

        if (remaining_qty > 0) {
            // Use incoming price as resting price
            sells_.push_back(incoming.price, BookOrder(incoming, remaining_qty));
        }
    }
}

} // namespace havarti
