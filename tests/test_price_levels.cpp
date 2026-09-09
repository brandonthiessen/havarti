#include "price_levels.h"

#include <gtest/gtest.h>

namespace havarti {

namespace {

BookOrder make_order(OrderId id, Quantity quantity = 100)
{
    return BookOrder{id, quantity};
}

} // namespace

TEST(PriceLevelsTest, EmptyInitially)
{
    PriceLevels<Side::BUY> levels;

    EXPECT_EQ(levels.best_price(), NO_PRICE);
}

TEST(PriceLevelsTest, PushAndFront)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1, 50));

    EXPECT_FALSE(levels.empty(100));

    const auto& order = levels.front(100);
    EXPECT_EQ(order.id, 1);
    EXPECT_EQ(order.remaining, 50);
}

TEST(PriceLevelsTest, MaintainsFifoOrderAtSamePrice)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1));
    levels.push_back(100, make_order(2));
    levels.push_back(100, make_order(3));

    EXPECT_EQ(levels.front(100).id, 1);

    levels.pop_front(100);
    EXPECT_EQ(levels.front(100).id, 2);

    levels.pop_front(100);
    EXPECT_EQ(levels.front(100).id, 3);
}

TEST(PriceLevelsTest, BestBuyPriceIsHighest)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1));
    levels.push_back(200, make_order(2));
    levels.push_back(150, make_order(3));

    EXPECT_EQ(levels.best_price(), 200);
}

TEST(PriceLevelsTest, BestSellPriceIsLowest)
{
    PriceLevels<Side::SELL> levels;

    levels.push_back(100, make_order(1));
    levels.push_back(200, make_order(2));
    levels.push_back(150, make_order(3));

    EXPECT_EQ(levels.best_price(), 100);
}

TEST(PriceLevelsTest, BestPriceUpdatesAfterRemovingLevel)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1));
    levels.push_back(200, make_order(2));
    levels.push_back(150, make_order(3));

    EXPECT_EQ(levels.best_price(), 200);

    levels.pop_front(200);

    EXPECT_EQ(levels.best_price(), 150);
}

TEST(PriceLevelsTest, EmptyPriceLevelAfterLastOrderIsRemoved)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1));

    EXPECT_FALSE(levels.empty(100));

    levels.pop_front(100);

    EXPECT_TRUE(levels.empty(100));
    EXPECT_EQ(levels.best_price(), NO_PRICE);
}

TEST(PriceLevelsTest, BestPriceHandlesMultipleOrdersAtBestPrice)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(100, make_order(1));
    levels.push_back(200, make_order(2));
    levels.push_back(200, make_order(3));
    levels.push_back(150, make_order(4));

    EXPECT_EQ(levels.best_price(), 200);

    levels.pop_front(200);

    EXPECT_EQ(levels.best_price(), 200);
    EXPECT_EQ(levels.front(200).id, 3);

    levels.pop_front(200);

    EXPECT_EQ(levels.best_price(), 150);
}

TEST(PriceLevelsTest, HandlesDensePriceLevels)
{
    PriceLevels<Side::BUY> levels;

    for (Price price = 1000; price < 1100; ++price) {
        levels.push_back(price, make_order(price));
    }

    EXPECT_EQ(levels.best_price(), 1099);
}

TEST(PriceLevelsTest, HandlesPricesBelowDenseRange)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(5000, make_order(1));
    levels.push_back(1000, make_order(2));

    EXPECT_EQ(levels.best_price(), 5000);

    levels.pop_front(5000);

    EXPECT_EQ(levels.best_price(), 1000);
}

TEST(PriceLevelsTest, HandlesPricesAboveDenseRange)
{
    PriceLevels<Side::BUY> levels;

    levels.push_back(1000, make_order(1));
    levels.push_back(6000, make_order(2));

    EXPECT_EQ(levels.best_price(), 6000);

    levels.pop_front(6000);

    EXPECT_EQ(levels.best_price(), 1000);
}

TEST(PriceLevelsTest, SellHandlesHighOutlierPrices)
{
    PriceLevels<Side::SELL> levels;

    levels.push_back(1000, make_order(1));
    levels.push_back(6000, make_order(2));

    EXPECT_EQ(levels.best_price(), 1000);

    levels.pop_front(1000);

    EXPECT_EQ(levels.best_price(), 6000);
}

} // namespace havarti
