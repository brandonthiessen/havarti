#pragma once

#include "order_pool.h"

#include <cstdint>

namespace havarti {

class PriceLevel {
public:
    void push_back(OrderPool& pool, uint32_t index);

    BookOrder& front(OrderPool& pool);
    const BookOrder& front(const OrderPool& pool) const;

    uint32_t pop_front(OrderPool& pool);

    void erase(OrderPool& pool, uint32_t index);

    bool empty() const {
        return head_ == OrderPool::INVALID;
    }

private:
    uint32_t head_ = OrderPool::INVALID;
    uint32_t tail_ = OrderPool::INVALID;
};

} // namespace havarti
