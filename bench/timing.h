#pragma once

#include <chrono>

namespace havarti::bench {
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    inline TimePoint now() {
        return Clock::now();
    }

    inline std::chrono::nanoseconds diff(TimePoint start, TimePoint end) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    }

    inline double to_seconds(std::chrono::nanoseconds ns) {
        return std::chrono::duration<double>(ns).count();
    }
} // namespace havarti::bench
