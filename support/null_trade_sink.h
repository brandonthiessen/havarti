#pragma once

#include "trade.h"
#include "trade_sink.h"

namespace havarti::support {

class NullTradeSink final : public havarti::TradeSink {
    public:
        bool submit(const havarti::Trade& trade);
};

} // namespace havarti::support
