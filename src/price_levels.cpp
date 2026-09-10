#include "order.h"
#include "order_pool.h"
#include "price_level.h"
#include "price_levels.h"

#include <bit>
#include <cassert>
#include <cstdint>
#include <sys/types.h>

namespace havarti {

template <Side side>
PriceLevels<side>::PriceLevels() : occupied_{}, occupied_words_{}
{}

template <Side side>
void
PriceLevels<side>::push_back(Price price, BookOrder order)
{
    if (!initialized_) {
        // Force invariant: dense_min_ >= 0
        const Price half = dense_size / 2;
        dense_min_ = price >= half ? price - half : 0;
        initialized_ = true;
    }

    PriceLevel* level;

    if (price < dense_min_) {
        level = &low_[price];
    } else if (price >= dense_min_ + dense_size) {
        level = &high_[price];
    } else {

        size_t idx = price - dense_min_;
        size_t word = idx / word_size;
        size_t bit = idx % word_size;

        if (occupied_[word] == 0) {
            occupied_words_ |= uint64_t{1} << word;
        }

        occupied_[word] |= uint64_t{1} << bit;
        level = &dense_[idx];
    }

    const uint32_t index = pool_.allocate(std::move(order), price);

    level->push_back(pool_, index);

    order_index_.set(pool_[index].order.id, index);
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

        // Search from the high side of the bitmap
        if (occupied_words_ != 0) {
            auto word = 63 - std::countl_zero(occupied_words_);
            auto bits = occupied_[word];

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

        // Search from the low side of the bitmap
        if (occupied_words_ != 0) {
            auto word = std::countr_zero(occupied_words_);
            auto bits = occupied_[word];

            auto bit = std::countr_zero(bits);
            return dense_min_ + word * word_size + bit;
        }

        // Check high outlier map
        if (!high_.empty()) {
            return high_.begin()->first;
        }
    }

    return NO_PRICE;
}

template <Side side>
BookOrder&
PriceLevels<side>::front(Price price)
{
    // Pricing outliers
    PriceLevel* level;

    if (price < dense_min_) {
        level = &low_.at(price);
    } else if (price >= dense_min_ + dense_size) {
        level = &high_.at(price);
    } else {
        const size_t idx = price - dense_min_;
        level = &dense_[idx];
    }

    return level->front(pool_);
}

template <Side side>
void
PriceLevels<side>::pop_front(Price price)
{
    PriceLevel* level;
    bool dense = false;

    if (price < dense_min_) {
        level = &low_.at(price);

    } else if (price >= dense_min_ + dense_size) {
        level = &high_.at(price);

    } else {
        const size_t idx = price - dense_min_;
        level = &dense_[idx];
        dense = true;
    }

    const uint32_t index = level->pop_front(pool_);

    const OrderId id = pool_[index].order.id;

    order_index_.set(id, OrderPool::INVALID);
    pool_.release(index);

    if (!level->empty()) return;

    // PriceLevel is now empty, update bitmaps
    if (dense) {
        const size_t idx = price - dense_min_;
        const size_t word = idx / word_size;
        const size_t bit = idx % word_size;

        occupied_[word] &= ~(uint64_t{1} << bit);

        if (occupied_[word] == 0) {
            occupied_words_ &= ~(uint64_t{1} << word);
        }
    } else if (price < dense_min_) {
        low_.erase(price);
    } else {
        high_.erase(price);
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

template <Side side>
bool
PriceLevels<side>::erase(const OrderId id)
{
    const auto index = order_index_.get(id);
    if (index == OrderPool::INVALID) return false;

    const Price price = pool_[index].price;

    PriceLevel* level;
    bool dense = false;

    if (price < dense_min_) {
        level = &low_.at(price);

    } else if (price >= dense_min_ + dense_size) {
        level = &high_.at(price);

    } else {
        const size_t idx = price - dense_min_;
        level = &dense_[idx];
        dense = true;
    }

    level->erase(pool_, index);

    order_index_.set(id, OrderPool::INVALID);
    pool_.release(index);

    if (!level->empty()) return true;

    // PriceLevel is now empty, update bitmap occupancy
    if (dense) {
        const size_t idx = price - dense_min_;
        const size_t word = idx / word_size;
        const size_t bit = idx % word_size;

        occupied_[word] &= ~(uint64_t{1} << bit);

        if (occupied_[word] == 0) {
            occupied_words_ &= ~(uint64_t{1} << word);
        }

    } else if (price < dense_min_) {
        low_.erase(price);

    } else {
        high_.erase(price);
    }

    return true;
}

template class PriceLevels<Side::BUY>;
template class PriceLevels<Side::SELL>;

} // namespace havarti
