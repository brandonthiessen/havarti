#pragma once

#include "order.h"

#include <array>
#include <cstdint>
#include <memory>

namespace havarti {

class OrderIndex {
    static constexpr uint32_t PAGE_BITS = 12;
    static constexpr uint32_t PAGE_SIZE = 1u << PAGE_BITS;
    static constexpr uint32_t PAGE_MASK = PAGE_SIZE - 1;

    static constexpr uint32_t LEVEL_BITS = 10;
    static constexpr uint32_t LEVEL_SIZE = 1u << LEVEL_BITS;
    static constexpr uint32_t LEVEL_MASK = LEVEL_SIZE - 1;

    static constexpr uint32_t INVALID = UINT32_MAX;

    using Page = std::array<uint32_t, PAGE_SIZE>;
    using PageTable = std::array<std::unique_ptr<Page>, LEVEL_SIZE>;

    public:
        uint32_t get(OrderId id) const noexcept;
        void set(OrderId id, uint32_t value);

    private:
        std::array<std::unique_ptr<PageTable>, LEVEL_SIZE> page_tables_{};
};
} // namespace havarti
