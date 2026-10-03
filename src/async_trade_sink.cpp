#include "async_trade_sink.h"
#include "ring_buffer.h"
#include "trade.h"

namespace havarti {

AsyncTradeSink::AsyncTradeSink()
{
    thread_ = std::thread(&AsyncTradeSink::run, this);
}

AsyncTradeSink::~AsyncTradeSink()
{
    shutdown();
}

bool
AsyncTradeSink::submit(const Trade& trade)
{
    if (!queue_.try_push(trade)) {
        trades_dropped_++;
        return false;
    }

    return true;
}

void
AsyncTradeSink::shutdown()
{
    running_.store(false, std::memory_order_relaxed);

    if (thread_.joinable()) {
        thread_.join();
    }
}

void
AsyncTradeSink::run()
{
    Trade trade;
    while (running_.load(std::memory_order_relaxed)) {
        while (queue_.try_pop(trade)) {
            // TODO: properly handle trades
            trades_processed_++;
        }
    }

    // Drain anything submitted before shutdown
    while (queue_.try_pop(trade)) {
        trades_processed_++;
    }
}

} // namespace havarti
