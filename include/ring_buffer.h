#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>

namespace havarti {

// A single-producer, single-consumer ring buffer
// The buffer has Capacity slots, of which Capacity - 1 are usable.
template <typename T, size_t Capacity>
struct SpscRingBuffer {
    // Queue capacity must be a power of 2.
    static_assert(Capacity > 0);
    static_assert((Capacity & (Capacity - 1)) == 0);

    SpscRingBuffer();
    ~SpscRingBuffer();

    // SPSC ring buffer represents a communication channel and cannot be copied or moved.
    SpscRingBuffer(const SpscRingBuffer&) = delete;
    SpscRingBuffer& operator=(const SpscRingBuffer&) = delete;
    SpscRingBuffer(SpscRingBuffer&&) = delete;
    SpscRingBuffer& operator=(SpscRingBuffer&&) = delete;

    bool try_push(const T& value);
    bool try_pop(T& out);
    bool try_peek(T& out);
    bool empty() const;

    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    T* buffer_{nullptr};
};

template <typename T, size_t Capacity>
SpscRingBuffer<T, Capacity>::SpscRingBuffer()
{
    buffer_ = new T[Capacity];
}

template <typename T, size_t Capacity>
SpscRingBuffer<T, Capacity>::~SpscRingBuffer() {
    delete[] buffer_;
}

template <typename T, size_t Capacity>
bool
SpscRingBuffer<T, Capacity>::try_push(const T& value)
{
    size_t head = head_.load(std::memory_order_relaxed);
    size_t next = (head + 1) & (Capacity - 1);

    // Check if buffer is full
    if (next == tail_.load(std::memory_order_acquire)) {
        return false;
    }

    buffer_[head] = value;
    head_.store(next, std::memory_order_release);
    return true;
}

template <typename T, size_t Capacity>
bool
SpscRingBuffer<T, Capacity>::try_pop(T& out)
{
    size_t tail = tail_.load(std::memory_order_relaxed);

    // Check if buffer is empty
    if (tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    out = buffer_[tail];
    tail_.store((tail + 1) & (Capacity - 1), std::memory_order_release);
    return true;
}

template <typename T, size_t Capacity>
bool
SpscRingBuffer<T, Capacity>::try_peek(T& out)
{
    size_t tail = tail_.load(std::memory_order_relaxed);

    // Check if buffer is empty
    if (tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    out = buffer_[tail];
    return true;
}

template <typename T, size_t Capacity>
bool
SpscRingBuffer<T, Capacity>::empty() const
{
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t head = head_.load(std::memory_order_relaxed);
    return head == tail;
}


} // namespace havarti
