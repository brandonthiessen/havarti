#include "price_levels.h"

#include <bit>
#include <cassert>
#include <cstdint>

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

    std::list<BookOrder>* queue;

    if (price < dense_min_) {
        queue = &low_[price];
    } else if (price >= dense_min_ + dense_size) {
        queue = &high_[price];
    } else {

        size_t idx = price - dense_min_;
        size_t word = idx / word_size;
        size_t bit = idx % word_size;

        if (occupied_[word] == 0) {
            occupied_words_ |= uint64_t{1} << word;
        }

        occupied_[word] |= uint64_t{1} << bit;
        queue = &dense_[idx];
    }

    queue->push_back(std::move(order));

    auto it = std::prev(queue->end());
    orders_[it->id] = {price, it};
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
        const OrderId id = queue.front().id;

        queue.pop_front();
        orders_.erase(id);

        if (queue.empty()) {
            low_.erase(price);
        }

        return;
    }

    if (price >= dense_min_ + dense_size) {
        auto& queue = high_.at(price);
        const OrderId id = queue.front().id;

        queue.pop_front();
        orders_.erase(id);

        if (queue.empty()) {
            high_.erase(price);
        }

        return;
    }

    size_t idx = price - dense_min_;
    assert(!dense_[idx].empty());

    auto& queue = dense_[idx];
    const OrderId id = queue.front().id;

    queue.pop_front();
    orders_.erase(id);

    if (dense_[idx].empty()) {
        // Indicate that this price level is empty
        size_t word = idx / word_size;
        size_t bit = idx % word_size;
        occupied_[word] &= ~(uint64_t{1} << bit);

        if (occupied_[word] == 0) {
            occupied_words_ &= ~(uint64_t{1} << word);
        }
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
    const auto map_it = orders_.find(id);
    if (map_it == orders_.end()) return false;

    const auto& [price, list_it] = map_it->second;

    if (price < dense_min_) {
        auto& queue = low_.at(price);
        queue.erase(list_it);

        if (queue.empty()) {
            low_.erase(price);
        }

    } else if (price >= dense_min_ + dense_size) {
        auto& queue = high_.at(price);
        queue.erase(list_it);

        if (queue.empty()) {
            high_.erase(price);
        }

    } else {
        size_t idx = price - dense_min_;
        dense_[idx].erase(list_it);
        if (dense_[idx].empty()) {
            size_t word = idx / word_size;
            size_t bit = idx % word_size;

            occupied_[word] &= ~(uint64_t{1} << bit);

            if (occupied_[word] == 0) {
                occupied_words_ &= ~(uint64_t{1} << word);
            }
        }

    }
    
    orders_.erase(map_it);
    return true;
}

template class PriceLevels<Side::BUY>;
template class PriceLevels<Side::SELL>;

} // namespace havarti
