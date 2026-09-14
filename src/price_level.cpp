#include "price_level.h"

#include <cassert>

namespace havarti {

void
PriceLevel::push_back(OrderPool& pool, uint32_t index)
{
    auto& node = pool[index];

    node.prev = tail_;
    node.next = OrderPool::INVALID;

    if (tail_ != OrderPool::INVALID) {
        pool[tail_].next = index;
    } else {
        // Queue was empty.
        head_ = index;
    }

    tail_ = index;
}

BookOrder&
PriceLevel::front(OrderPool& pool)
{
    assert(head_ != OrderPool::INVALID);
    return pool[head_].order;
}

const BookOrder&
PriceLevel::front(const OrderPool& pool) const
{
    assert(head_ != OrderPool::INVALID);
    return pool[head_].order;
}

uint32_t
PriceLevel::pop_front(OrderPool& pool)
{
    assert(head_ != OrderPool::INVALID);

    const uint32_t index = head_;
    auto& node = pool[index];

    head_ = node.next;

    if (head_ != OrderPool::INVALID) {
        pool[head_].prev = OrderPool::INVALID;
    } else {
        // Queue is now empty.
        tail_ = OrderPool::INVALID;
    }

    return index;
}

void
PriceLevel::erase(OrderPool& pool, uint32_t index)
{
    auto& node = pool[index];

    const uint32_t prev = node.prev;
    const uint32_t next = node.next;

    if (prev != OrderPool::INVALID) {
        pool[prev].next = next;
    } else {
        // Removing head.
        head_ = next;
    }

    if (next != OrderPool::INVALID) {
        pool[next].prev = prev;
    } else {
        // Removing tail.
        tail_ = prev;
    }
}

} // namespace havarti
