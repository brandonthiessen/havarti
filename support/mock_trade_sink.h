#pragma once

#include "trade.h"
#include "trade_sink.h"

#include <vector>

namespace test {

class MockTradeSink final : public havarti::TradeSink {
    public:
        bool submit(const havarti::Trade& trade);

        std::vector<havarti::Trade> trades;
};

} // namespace test
