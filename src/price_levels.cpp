#include "price_levels.h"

#include <algorithm>
#include <bit>
#include <cassert>

namespace havarti {

template <Side side>
PriceLevels<side>::PriceLevels() : occupied_{}
{}

template <Side side>
void
PriceLevels<side>::push_back(Price price, BookOrder order)
{
    if (!initialized_) {
        // Force invariant: dense_min_ >= 0
        dense_min_ = std::max<Price>(0, price - static_cast<Price>(dense_size / 2));
        initialized_ = true;
    }

    if (price < dense_min_) {
        low_[price].push_back(std::move(order));
        return;
    } else if (price >= dense_min_ + dense_size) {
        high_[price].push_back(std::move(order));
        return;
    }

    size_t idx = price - dense_min_;
    size_t word = idx / word_size;
    size_t bit = idx % word_size;

    occupied_[word] |= uint64_t{1} << bit;
    dense_[idx].push_back(std::move(order));
}

template <Side side>
Price
PriceLevels<side>::best_price(Price bound) const
{
    if (bound == NO_PRICE)
        return best_price();

    Price dense_max = dense_min_ + dense_size;

    if constexpr (side == Side::BUY) {
        // Find highest price <= bound
        if (bound < dense_min_) {
            // Only low_ can contain a qualifying price
            auto it = low_.upper_bound(bound);

            if (it != low_.begin()) {
                return std::prev(it)->first;
            }

            return 0;
        }

        // First check high_ if the bound is above the dense region
        if (bound >= dense_max && !high_.empty()) {
            auto it = high_.upper_bound(bound);

            if (it != high_.begin()) {
                return std::prev(it)->first;
            }
        }

        // Search dense_
        size_t idx = bound >= dense_max ? dense_size - 1 : static_cast<size_t>(bound - dense_min_);
        size_t word = idx / word_size;
        size_t bit = idx % word_size;

        // Keep bits <= bound
        uint64_t bits = occupied_[word];

        if (bit != word_size - 1) {
            bits &= (uint64_t{1} << (bit + 1)) - 1;
        }

        if (bits != 0) {
            auto offset = 63 - std::countl_zero(bits);
            return dense_min_ + word * word_size + offset;
        }

        // Search preceding words
        for (size_t w = word; w-- > 0;) {
            bits = occupied_[w];

            if (bits == 0)
                continue;

            auto offset = 63 - std::countl_zero(bits);
            return dense_min_ + w * word_size + offset;
        }

        // Nothing in dense_ <= bound, so fall back to low_
        if (!low_.empty()) {
            return low_.rbegin()->first;
        }

        return 0;

    } else {
        // Find lowest price >= bound
        if (bound >= dense_max) {
            // Only high_ can contain a qualifying price
            auto it = high_.lower_bound(bound);

            if (it != high_.end()) {
                return it->first;
            }

            return 0;
        }

        // First check low_ if the bound is below the dense region
        if (bound < dense_min_ && !low_.empty()) {
            auto it = low_.lower_bound(bound);

            if (it != low_.end()) {
                return it->first;
            }
        }

        // Search dense_
        size_t idx = bound <= dense_min_ ? 0 : static_cast<size_t>(bound - dense_min_);
        size_t word = idx / word_size;
        size_t bit = idx % word_size;

        // Keep bits >= bound
        uint64_t bits = occupied_[word] & (~uint64_t{0} << bit);

        if (bits != 0) {
            auto offset = std::countr_zero(bits);
            return dense_min_ + word * word_size + offset;
        }

        // Search following words
        for (size_t w = word + 1; w < occupied_.size(); ++w) {
            bits = occupied_[w];

            if (bits == 0)
                continue;

            auto offset = std::countr_zero(bits);
            return dense_min_ + w * word_size + offset;
        }

        // Nothing in dense_ >= bound, so fall back to high_
        if (!high_.empty()) {
            return high_.begin()->first;
        }

        return 0;
    }
}

template <Side side>
Price
PriceLevels<side>::best_price() const
{
    if constexpr (side == Side::BUY) {
        // Check high outlier map
        if (!high_.empty()) {
            return high_.rbegin()->first;
        }

        // Start iterating from the high side of the bitmap
        for (size_t word = occupied_.size(); word-- > 0;) {
            uint64_t bits = occupied_[word];

            if (bits == 0)
                continue;

            auto bit = 63 - std::countl_zero(bits);
            return dense_min_ + word * word_size + bit;
        }

        // Check low outlier map
        if (!low_.empty()) {
            return low_.rbegin()->first;
        }

    } else {
        // Check low outlier map
        if (!low_.empty()) {
            return low_.begin()->first;
        }

        // Iterating from the low side of the bitmap
        for (size_t word = 0; word < occupied_.size(); word++) {
            uint64_t bits = occupied_[word];

            if (bits == 0)
                continue;

            auto bit = std::countr_zero(bits);
            return dense_min_ + word * word_size + bit;
        }

        // Check high outlier map
        if (!high_.empty()) {
            return high_.begin()->first;
        }
    }

    return 0;
}

template <Side side>
BookOrder&
PriceLevels<side>::front(Price price)
{
    // Pricing outliers
    if (price < dense_min_) return low_.at(price).front();
    if (price >= dense_min_ + dense_size) return high_.at(price).front();

    size_t idx = price - dense_min_;
    assert(!dense_[idx].empty());
    return dense_[idx].front();
}

template <Side side>
void
PriceLevels<side>::pop_front(Price price)
{
    // Pricing outliers
    if (price < dense_min_) {
        auto& queue = low_.at(price);
        queue.pop_front();

        if (queue.empty()) {
            low_.erase(price);
        }

        return;
    }

    if (price >= dense_min_ + dense_size) {
        auto& queue = high_.at(price);
        queue.pop_front();

        if (queue.empty()) {
            high_.erase(price);
        }

        return;
    }

    size_t idx = price - dense_min_;
    assert(!dense_[idx].empty());
    dense_[idx].pop_front();

    if (dense_[idx].empty()) {
        // Indicate that this price level is empty
        size_t word = idx / word_size;
        size_t bit = idx % word_size;
        occupied_[word] &= ~(uint64_t{1} << bit);
    }
}

template <Side side>
bool
PriceLevels<side>::empty(Price price) const
{
    if (price < dense_min_) {
        return !low_.contains(price);
    } else if (price >= dense_min_ + dense_size) {
        return !high_.contains(price);
    }

    size_t idx = price - dense_min_;
    return dense_[idx].empty();
}

template class PriceLevels<Side::BUY>;
template class PriceLevels<Side::SELL>;

} // namespace havarti
