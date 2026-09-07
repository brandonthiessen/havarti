#include "mock_trade_sink.h"
#include "trade.h"

namespace test {

bool
MockTradeSink::submit(const havarti::Trade& trade)
{
    trades.push_back(trade);
    return true;
}

} // namespace test
