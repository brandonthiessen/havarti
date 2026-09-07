#include "async_trade_sink.h"
#include "ring_buffer.h"
#include "trade.h"

namespace havarti {

AsyncTradeSink::AsyncTradeSink(const size_t queue_capacity):
    queue_(queue_capacity),
    thread_(&AsyncTradeSink::run, this)
{}

AsyncTradeSink::~AsyncTradeSink()
{
    running_.store(false, std::memory_order_relaxed);
    thread_.join();
}

bool
AsyncTradeSink::submit(const Trade& trade) {
    return queue_.try_push(trade);
}

void
AsyncTradeSink::run() {
    while (running_.load(std::memory_order_relaxed)) {
        Trade trade;
        while (queue_.try_pop(trade)) {
            // TODO: properly handle trades
            trades_processed++;
        }
    }

    // Drain anything submitted before shutdown
    Trade trade;
    while (queue_.try_pop(trade)) {
        trades_processed++;
    }
}

} // namespace havarti
