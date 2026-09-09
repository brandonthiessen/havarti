#include "orderbook.h"
#include "mock_trade_sink.h"

#include <gtest/gtest.h>

namespace havarti {

namespace {

Order make_order(
    OrderId id,
    Price price,
    Quantity quantity,
    Side side)
{
    return Order{id, price, quantity, side};
}

} // namespace

TEST(OrderBookTest, OrderRestsWhenThereIsNoMatch)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 50, Side::BUY));

    EXPECT_TRUE(sink.trades.empty());
}

TEST(OrderBookTest, ExactBuySellMatch)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 50, Side::BUY));
    book.add_order(make_order(2, 100, 50, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 1);

    const Trade& trade = sink.trades[0];

    EXPECT_EQ(trade.buy_order_id, 1);
    EXPECT_EQ(trade.sell_order_id, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 50);
}

TEST(OrderBookTest, BuyMatchesSellAtLowerPrice)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 90, 50, Side::SELL));
    book.add_order(make_order(2, 100, 50, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 1);

    const Trade& trade = sink.trades[0];

    EXPECT_EQ(trade.buy_order_id, 2);
    EXPECT_EQ(trade.sell_order_id, 1);
    EXPECT_EQ(trade.price, 90);
    EXPECT_EQ(trade.quantity, 50);
}

TEST(OrderBookTest, NonCrossingOrdersDoNotMatch)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 90, 50, Side::BUY));
    book.add_order(make_order(2, 100, 50, Side::SELL));

    EXPECT_TRUE(sink.trades.empty());
}

TEST(OrderBookTest, IncomingBuyPartiallyFillsRestingSell)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 50, Side::SELL));
    book.add_order(make_order(2, 100, 30, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].buy_order_id, 2);
    EXPECT_EQ(sink.trades[0].sell_order_id, 1);
    EXPECT_EQ(sink.trades[0].price, 100);
    EXPECT_EQ(sink.trades[0].quantity, 30);

    // The remaining 20 from order 1 should still be available.
    book.add_order(make_order(3, 100, 20, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 2);
    EXPECT_EQ(sink.trades[1].buy_order_id, 3);
    EXPECT_EQ(sink.trades[1].sell_order_id, 1);
    EXPECT_EQ(sink.trades[1].quantity, 20);
}

TEST(OrderBookTest, IncomingBuyPartiallyFillsThenRests)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 30, Side::SELL));
    book.add_order(make_order(2, 100, 50, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].quantity, 30);

    // Remaining 20 from the incoming buy should rest.
    book.add_order(make_order(3, 100, 20, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 2);
    EXPECT_EQ(sink.trades[1].buy_order_id, 2);
    EXPECT_EQ(sink.trades[1].sell_order_id, 3);
    EXPECT_EQ(sink.trades[1].quantity, 20);
}

TEST(OrderBookTest, IncomingSellPartiallyFillsRestingBuy)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 50, Side::BUY));
    book.add_order(make_order(2, 100, 30, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].buy_order_id, 1);
    EXPECT_EQ(sink.trades[0].sell_order_id, 2);
    EXPECT_EQ(sink.trades[0].quantity, 30);

    // The remaining 20 from order 1 should still be available.
    book.add_order(make_order(3, 100, 20, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 2);
    EXPECT_EQ(sink.trades[1].buy_order_id, 1);
    EXPECT_EQ(sink.trades[1].sell_order_id, 3);
    EXPECT_EQ(sink.trades[1].quantity, 20);
}

TEST(OrderBookTest, BuyUsesPricePriority)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 90, 50, Side::SELL));
    book.add_order(make_order(2, 100, 50, Side::SELL));

    book.add_order(make_order(3, 110, 50, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].sell_order_id, 1);
    EXPECT_EQ(sink.trades[0].price, 90);
}

TEST(OrderBookTest, SellUsesPricePriority)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 110, 50, Side::BUY));
    book.add_order(make_order(2, 100, 50, Side::BUY));

    book.add_order(make_order(3, 90, 50, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].buy_order_id, 1);
    EXPECT_EQ(sink.trades[0].price, 110);
}

TEST(OrderBookTest, MaintainsTimePriorityAtSamePrice)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 100, 50, Side::SELL));
    book.add_order(make_order(2, 100, 50, Side::SELL));

    book.add_order(make_order(3, 100, 75, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 2);

    EXPECT_EQ(sink.trades[0].sell_order_id, 1);
    EXPECT_EQ(sink.trades[0].quantity, 50);

    EXPECT_EQ(sink.trades[1].sell_order_id, 2);
    EXPECT_EQ(sink.trades[1].quantity, 25);
}

TEST(OrderBookTest, IncomingOrderCanMatchMultiplePriceLevels)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 90, 20, Side::SELL));
    book.add_order(make_order(2, 95, 30, Side::SELL));
    book.add_order(make_order(3, 100, 40, Side::SELL));

    book.add_order(make_order(4, 100, 70, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 3);

    EXPECT_EQ(sink.trades[0].sell_order_id, 1);
    EXPECT_EQ(sink.trades[0].price, 90);
    EXPECT_EQ(sink.trades[0].quantity, 20);

    EXPECT_EQ(sink.trades[1].sell_order_id, 2);
    EXPECT_EQ(sink.trades[1].price, 95);
    EXPECT_EQ(sink.trades[1].quantity, 30);

    EXPECT_EQ(sink.trades[2].sell_order_id, 3);
    EXPECT_EQ(sink.trades[2].price, 100);
    EXPECT_EQ(sink.trades[2].quantity, 20);
}

TEST(OrderBookTest, IncomingSellCanMatchMultiplePriceLevels)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 110, 20, Side::BUY));
    book.add_order(make_order(2, 105, 30, Side::BUY));
    book.add_order(make_order(3, 100, 40, Side::BUY));

    book.add_order(make_order(4, 100, 70, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 3);

    EXPECT_EQ(sink.trades[0].buy_order_id, 1);
    EXPECT_EQ(sink.trades[0].price, 110);
    EXPECT_EQ(sink.trades[0].quantity, 20);

    EXPECT_EQ(sink.trades[1].buy_order_id, 2);
    EXPECT_EQ(sink.trades[1].price, 105);
    EXPECT_EQ(sink.trades[1].quantity, 30);

    EXPECT_EQ(sink.trades[2].buy_order_id, 3);
    EXPECT_EQ(sink.trades[2].price, 100);
    EXPECT_EQ(sink.trades[2].quantity, 20);
}

TEST(OrderBookTest, RemainingBuyQuantityRestsAtIncomingPrice)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 90, 20, Side::SELL));
    book.add_order(make_order(2, 100, 50, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].quantity, 20);

    // The remaining 30 from order 2 must rest at 100.
    book.add_order(make_order(3, 100, 30, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 2);
    EXPECT_EQ(sink.trades[1].buy_order_id, 2);
    EXPECT_EQ(sink.trades[1].sell_order_id, 3);
    EXPECT_EQ(sink.trades[1].price, 100);
    EXPECT_EQ(sink.trades[1].quantity, 30);
}

TEST(OrderBookTest, RemainingSellQuantityRestsAtIncomingPrice)
{
    support::MockTradeSink sink;
    OrderBook book(sink);

    book.add_order(make_order(1, 110, 20, Side::BUY));
    book.add_order(make_order(2, 100, 50, Side::SELL));

    ASSERT_EQ(sink.trades.size(), 1);
    EXPECT_EQ(sink.trades[0].quantity, 20);

    // The remaining 30 from order 2 must rest at 100.
    book.add_order(make_order(3, 100, 30, Side::BUY));

    ASSERT_EQ(sink.trades.size(), 2);
    EXPECT_EQ(sink.trades[1].buy_order_id, 3);
    EXPECT_EQ(sink.trades[1].sell_order_id, 2);
    EXPECT_EQ(sink.trades[1].price, 100);
    EXPECT_EQ(sink.trades[1].quantity, 30);
}

} // namespace havarti
