#include "async_trade_sink.h"

#include <gtest/gtest.h>

namespace havarti {

namespace {

Trade make_trade(
    OrderId buy_order_id,
    OrderId sell_order_id,
    Price price = 100,
    Quantity quantity = 50)
{
    return Trade{
        buy_order_id,
        sell_order_id,
        price,
        quantity
    };
}

} // namespace

TEST(AsyncTradeSinkTest, ProcessesSubmittedTrade)
{
    AsyncTradeSink sink(16);

    ASSERT_TRUE(sink.submit(make_trade(1, 2)));

    sink.shutdown();

    EXPECT_EQ(sink.get_trades_processed(), 1);
    EXPECT_EQ(sink.get_trades_dropped(), 0);
}

TEST(AsyncTradeSinkTest, ProcessesMultipleSubmittedTrades)
{
    AsyncTradeSink sink(1024);

    constexpr int num_trades = 1000;

    for (int i = 0; i < num_trades; ++i) {
        ASSERT_TRUE(sink.submit(make_trade(i, i + 1)));
    }

    sink.shutdown();

    EXPECT_EQ(sink.get_trades_processed(), num_trades);
    EXPECT_EQ(sink.get_trades_dropped(), 0);
}

TEST(AsyncTradeSinkTest, ShutdownIsIdempotent)
{
    AsyncTradeSink sink(16);

    ASSERT_TRUE(sink.submit(make_trade(1, 2)));

    sink.shutdown();
    sink.shutdown();

    EXPECT_EQ(sink.get_trades_processed(), 1);
    EXPECT_EQ(sink.get_trades_dropped(), 0);
}

TEST(AsyncTradeSinkTest, ShutdownDrainsQueuedTrades)
{
    AsyncTradeSink sink(8192);

    constexpr int num_trades = 1000;

    for (int i = 0; i < num_trades; ++i) {
        ASSERT_TRUE(sink.submit(make_trade(i, i + 1)));
    }

    sink.shutdown();

    // shutdown() guarantees that all successfully submitted trades
    // have been processed before it returns.
    EXPECT_EQ(sink.get_trades_processed(), num_trades);
}

TEST(AsyncTradeSinkTest, HandlesDifferentTradeValues)
{
    AsyncTradeSink sink(16);

    ASSERT_TRUE(sink.submit(make_trade(1, 2, 100, 10)));
    ASSERT_TRUE(sink.submit(make_trade(3, 4, 250, 75)));
    ASSERT_TRUE(sink.submit(make_trade(5, 6, 999, 1)));

    sink.shutdown();

    EXPECT_EQ(sink.get_trades_processed(), 3);
    EXPECT_EQ(sink.get_trades_dropped(), 0);
}

TEST(AsyncTradeSinkTest, DropsTradesWhenQueueIsFull)
{
    AsyncTradeSink sink(2);

    ASSERT_TRUE(sink.submit(make_trade(1, 2)));
    EXPECT_FALSE(sink.submit(make_trade(2, 3)));

    sink.shutdown();

    EXPECT_EQ(sink.get_trades_processed(), 1);
    EXPECT_EQ(sink.get_trades_dropped(), 1);
}

} // namespace havarti
