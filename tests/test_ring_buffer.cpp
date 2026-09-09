#include "ring_buffer.h"

#include <gtest/gtest.h>

namespace havarti {

TEST(RingBufferTest, IsEmptyInitially)
{
    SpscRingBuffer<int> buffer(4);

    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, PushThenPop)
{
    SpscRingBuffer<int> buffer(4);

    EXPECT_TRUE(buffer.try_push(42));

    int value;
    EXPECT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 42);
}

TEST(RingBufferTest, PopOnEmptyBufferReturnsFalse)
{
    SpscRingBuffer<int> buffer(4);

    int value;
    EXPECT_FALSE(buffer.try_pop(value));
}

TEST(RingBufferTest, MaintainsFifoOrder)
{
    SpscRingBuffer<int> buffer(4);

    ASSERT_TRUE(buffer.try_push(1));
    ASSERT_TRUE(buffer.try_push(2));
    ASSERT_TRUE(buffer.try_push(3));

    int value;

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 1);

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 2);

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, PeekReturnsFrontWithoutRemoving)
{
    SpscRingBuffer<int> buffer(4);

    ASSERT_TRUE(buffer.try_push(42));

    int value;
    ASSERT_TRUE(buffer.try_peek(value));
    EXPECT_EQ(value, 42);

    ASSERT_TRUE(buffer.try_peek(value));
    EXPECT_EQ(value, 42);

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 42);
}

TEST(RingBufferTest, CapacityIsOneLessThanBufferSize)
{
    SpscRingBuffer<int> buffer(4);

    EXPECT_TRUE(buffer.try_push(1));
    EXPECT_TRUE(buffer.try_push(2));
    EXPECT_TRUE(buffer.try_push(3));
    EXPECT_FALSE(buffer.try_push(4));
}

TEST(RingBufferTest, PushSucceedsAfterPopFromFullBuffer)
{
    SpscRingBuffer<int> buffer(4);

    ASSERT_TRUE(buffer.try_push(1));
    ASSERT_TRUE(buffer.try_push(2));
    ASSERT_TRUE(buffer.try_push(3));
    ASSERT_FALSE(buffer.try_push(4));

    int value;
    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 1);

    ASSERT_TRUE(buffer.try_push(4));

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 2);

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 3);

    ASSERT_TRUE(buffer.try_pop(value));
    EXPECT_EQ(value, 4);

    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, HandlesWraparound)
{
    SpscRingBuffer<int> buffer(4);

    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(buffer.try_push(i));

        int value;
        ASSERT_TRUE(buffer.try_pop(value));
        EXPECT_EQ(value, i);
    }

    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTest, PeekOnEmptyBufferReturnsFalse)
{
    SpscRingBuffer<int> buffer(4);

    int value;
    EXPECT_FALSE(buffer.try_peek(value));
}

TEST(RingBufferTest, CapacityOneCannotStoreElements)
{
    SpscRingBuffer<int> buffer(1);

    EXPECT_TRUE(buffer.empty());
    EXPECT_FALSE(buffer.try_push(42));
}

} // namespace havarti
