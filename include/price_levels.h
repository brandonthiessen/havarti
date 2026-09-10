#pragma once

#include "order.h"
#include "order_pool.h"
#include "price_level.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>

namespace havarti {

template <Side side>
class PriceLevels {
    public:
        PriceLevels();

        // Price navigation
        Price best_price() const;
        Price best_price(Price price) const;

        // FIFO queue manipulation at a specific price
        void push_back(Price price, BookOrder order);
        BookOrder& front(Price price);
        void pop_front(Price price);
        bool empty(Price price) const;

        // Order erasure
        bool erase(OrderId id);

    private:
        static constexpr size_t dense_size = 4096;
        static constexpr size_t word_size = sizeof(uint64_t) * 8;

        std::array<PriceLevel, dense_size> dense_;
        std::array<uint64_t, dense_size / word_size> occupied_;
        uint64_t occupied_words_;
        Price dense_min_;

        // Maps are indexed by price (ascending)
        std::map<Price, PriceLevel> low_;
        std::map<Price, PriceLevel> high_;

        // Owns all BookOrder nodes in contiguous storage
        OrderPool pool_;

        // Maps OrderId -> pool index
        std::vector<uint32_t> orders_;

        bool initialized_ = false;
};

} // namespace havarti
