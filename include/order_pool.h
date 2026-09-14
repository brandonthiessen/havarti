#pragma once

#include "order.h"

#include <cstdint>
#include <vector>

namespace havarti {

class OrderPool {
    public:
        static constexpr uint32_t INVALID = UINT32_MAX;

        struct OrderNode {
            BookOrder order;
            Price price;
            uint32_t prev = INVALID;
            uint32_t next = INVALID;
        };

        uint32_t allocate(BookOrder order, Price price);
        void release(uint32_t index);
        OrderNode& operator[](uint32_t index);
        const OrderNode& operator[](uint32_t index) const;

    private:
        std::vector<OrderNode> nodes_;
        std::vector<uint32_t> free_;
};

} // namespace havarti
