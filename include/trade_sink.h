#pragma once

#include "ring_buffer.h"
#include "trade.h"

#include <atomic>
#include <cstddef>
#include <thread>

namespace havarti {

class TradeSink {
    public:
        TradeSink(const size_t queue_capacity);
        ~TradeSink();
        bool submit(const Trade& trade);

    private:
        void run();

        int trades_processed;

        std::thread thread_;
        SpscRingBuffer<Trade> queue_;
        std::atomic<bool> running_{true};
};

} // namespace havarti
