#include "order.h"
#include "order_index.h"

#include <cstdint>
#include <memory>

namespace havarti {

uint32_t
OrderIndex::get(OrderId id) const noexcept
{
    const auto* page_table = page_tables_[id >> (PAGE_BITS + LEVEL_BITS)].get();
    if (!page_table) return INVALID;

    const auto* page = (*page_table)[(id >> PAGE_BITS) & LEVEL_MASK].get();
    if (!page) return INVALID;

    return (*page)[id & PAGE_MASK];
}

void
OrderIndex::set(OrderId id, uint32_t value)
{
    auto& page_table = page_tables_[id >> (PAGE_BITS + LEVEL_BITS)];
    if (!page_table) {
        page_table = std::make_unique<PageTable>();
    }

    auto& page = (*page_table)[(id >> PAGE_BITS) & LEVEL_MASK];
    if (!page) {
        page = std::make_unique<Page>();
    }

    (*page)[id & PAGE_MASK] = value;
}

} // namespace havarti
