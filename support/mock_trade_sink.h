#pragma once

#include "trade.h"
#include "trade_sink.h"

#include <vector>

namespace havarti::support {

class MockTradeSink final : public havarti::TradeSink {
    public:
        bool submit(const havarti::Trade& trade);

        std::vector<havarti::Trade> trades;
};

} // namespace havarti::support
