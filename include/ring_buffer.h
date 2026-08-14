#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>

namespace havarti {

// A single-producer, single-consumer ring buffer
// Note: actual usable number of entries is `capacity - 1`.
template <typename T>
struct SpscRingBuffer {
    SpscRingBuffer(size_t capacity);
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

    const size_t capacity_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
    T* buffer_{nullptr};
};

template <typename T>
SpscRingBuffer<T>::SpscRingBuffer(size_t capacity):
    capacity_(capacity)
{
    assert(capacity_ > 0);
    buffer_ = new T[capacity_];
}

template <typename T>
SpscRingBuffer<T>::~SpscRingBuffer() {
    delete[] buffer_;
}

template <typename T>
bool
SpscRingBuffer<T>::try_push(const T& value)
{
    size_t head = head_.load(std::memory_order_relaxed);
    size_t next = (head + 1) % capacity_;

    // Check if buffer is full
    if (next == tail_.load(std::memory_order_acquire)) {
        return false;
    }

    buffer_[head] = value;
    head_.store(next, std::memory_order_release);
    return true;
}

template <typename T>
bool
SpscRingBuffer<T>::try_pop(T& out)
{
    size_t tail = tail_.load(std::memory_order_relaxed);

    // Check if buffer is empty
    if (tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    out = buffer_[tail];
    tail_.store((tail + 1) % capacity_, std::memory_order_release);
    return true;
}

template <typename T>
bool
SpscRingBuffer<T>::try_peek(T& out)
{
    size_t tail = tail_.load(std::memory_order_relaxed);

    // Check if buffer is empty
    if (tail == head_.load(std::memory_order_acquire)) {
        return false;
    }

    out = buffer_[tail];
    return true;
}

template <typename T>
bool
SpscRingBuffer<T>::empty() const
{
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t head = head_.load(std::memory_order_relaxed);
    return head == tail;
}


} // namespace havarti
