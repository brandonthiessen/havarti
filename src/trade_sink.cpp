#include "trade_sink.h"
#include "ring_buffer.h"
#include "trade.h"

namespace havarti {

TradeSink::TradeSink(const size_t queue_capacity):
    queue_(queue_capacity),
    thread_(&TradeSink::run, this)
{
}

TradeSink::~TradeSink()
{
    while (!queue_.empty()) {
        std::this_thread::yield();
    }
    running_ = false;
    thread_.join();
}

bool
TradeSink::submit(const Trade& trade) {
    return queue_.try_push(trade);
}

void
TradeSink::run() {
    while (running_) {
        Trade trade;
        while (queue_.try_pop(trade)) {
            // TODO: properly handle trades
            trades_processed++;
        }
    }
}

} // namespace havarti
