#pragma once

#include "order.h"

#include <array>
#include <cstddef>
#include <deque>
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

    private:
        static constexpr size_t dense_size = 4096;
        static constexpr size_t word_size = sizeof(Price) * 8;
        std::array<std::deque<BookOrder>, dense_size> dense_;
        std::array<uint64_t, dense_size / word_size> occupied_;
        Price dense_min_;

        // Maps are indexed by price (ascending)
        std::map<Price, std::deque<BookOrder>> low_;
        std::map<Price, std::deque<BookOrder>> high_;

        bool initialized_ = false;
};

} // namespace havarti
