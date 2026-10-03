#include "null_trade_sink.h"
#include "trade.h"

namespace havarti::support {

bool
NullTradeSink::submit(const havarti::Trade& trade)
{
    return true;
}

} // namespace havarti::support
