#pragma once

#include <cstdint>

namespace havarti {

using Price = uint32_t;
using Quantity = uint32_t;
using OrderId = uint32_t;

constexpr Price NO_PRICE = 0;

enum class Side : std::uint8_t {
    BUY, SELL
};

struct Order {
    OrderId id;
    Price price;
    Quantity quantity;
    Side side;
};

struct BookOrder {
    OrderId id;
    Quantity remaining;
};

} // namespace havarti
