#include "order_pool.h"
#include "order.h"

#include <cassert>

namespace havarti {

uint32_t
OrderPool::allocate(BookOrder order, Price price)
{
    uint32_t index;

    if (!free_.empty()) {
        index = free_.back();
        free_.pop_back();

        nodes_[index] = OrderNode{order, price, INVALID, INVALID};
    } else {
        index = static_cast<uint32_t>(nodes_.size());

        nodes_.push_back(OrderNode{order, price, INVALID, INVALID});
    }

    return index;
}

void
OrderPool::release(uint32_t index) {
    assert(index < nodes_.size());

    free_.push_back(index);
}

OrderPool::OrderNode&
OrderPool::operator[](uint32_t index)
{
    assert(index < nodes_.size());

    return nodes_[index];
}

const OrderPool::OrderNode&
OrderPool::operator[](uint32_t index) const
{
    assert(index < nodes_.size());

    return nodes_[index];
}

}; // namespace havarti
