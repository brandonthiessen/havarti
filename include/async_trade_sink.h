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
        explicit AsyncTradeSink(const size_t queue_capacity);

        // AsyncTradeSink represents a communication channel and cannot be copied or moved.
        AsyncTradeSink(const AsyncTradeSink&) = delete;
        AsyncTradeSink& operator=(const AsyncTradeSink&) = delete;
        AsyncTradeSink(AsyncTradeSink&&) = delete;
        AsyncTradeSink& operator=(AsyncTradeSink&&) = delete;

        ~AsyncTradeSink();
        bool submit(const Trade& trade);

    private:
        void run();

        int trades_processed;

        std::thread thread_;
        SpscRingBuffer<Trade> queue_;
        std::atomic<bool> running_{true};
};

} // namespace havarti
