#include "mock_trade_sink.h"
#include "trade.h"

namespace havarti::support {

bool
MockTradeSink::submit(const havarti::Trade& trade)
{
    trades.push_back(trade);
    return true;
}

} // namespace havarti::support
