#pragma once

#include "ring_buffer.h"
#include "trade.h"
#include "trade_sink.h"

#include <atomic>
#include <cstddef>
#include <thread>

namespace havarti {

class AsyncTradeSink final : public TradeSink {
    public:
        explicit AsyncTradeSink();

        // AsyncTradeSink represents a communication channel and cannot be copied or moved.
        AsyncTradeSink(const AsyncTradeSink&) = delete;
        AsyncTradeSink& operator=(const AsyncTradeSink&) = delete;
        AsyncTradeSink(AsyncTradeSink&&) = delete;
        AsyncTradeSink& operator=(AsyncTradeSink&&) = delete;

        ~AsyncTradeSink();
        bool submit(const Trade& trade);

        void shutdown();

        int get_trades_processed() const { return trades_processed_; }
        int get_trades_dropped() const { return trades_dropped_; }

    private:
        void run();

        // Must be a power of 2.
        static constexpr size_t QUEUE_CAPACITY = 8192;
        SpscRingBuffer<Trade, QUEUE_CAPACITY> queue_;

        std::thread thread_;
        std::atomic<bool> running_{true};

        int trades_processed_{0};
        int trades_dropped_{0};
};

} // namespace havarti
