#pragma once

#include "order.h"

namespace havarti {

struct Trade {
    OrderId buy_order_id;
    OrderId sell_order_id;
    Price price;
    Quantity quantity;
};

} // namespace havarti
