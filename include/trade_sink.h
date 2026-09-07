#pragma once

#include "trade.h"

namespace havarti {

class TradeSink {
    public:
        virtual ~TradeSink() = default;

        virtual bool submit(const Trade& trade) = 0;
};

} // namespace havarti
