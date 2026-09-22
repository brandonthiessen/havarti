# Havarti

Havarti is a high-performance limit order book and matching engine written in C++20.

It implements **price-time priority matching**, FIFO order queues, cache-friendly price-level indexing, order cancellation, pooled order storage, and a pluggable trade sink with a lock-free SPSC async consumer. The project also includes a synthetic order generator (with configurable price and quantity distributions), a GoogleTest suite, and several benchmarks for measuring throughput and latency under different market conditions.

## Features

* Price-time priority matching
* FIFO queues at each price level
* Dense price-level indexing using hierarchical occupancy bitmaps
* `std::map` fallback for prices outside the dense range
* Pooled contiguous order storage with intrusive per-level queues
* Paginated `OrderIndex` for order lookup and cancellation by ID
* Order cancellation (`OrderBook::cancel_order`)
* Pluggable `TradeSink` interface; `AsyncTradeSink` with lock-free SPSC queue and background consumer thread
* Deterministic synthetic order generation with optional custom distributions
* Throughput and latency benchmarks (including p99.9 and p99.99 latency percentiles)
* GoogleTest unit test suite (order book, price levels, ring buffer, async trade sink)
* C++20 / CMake build system

## Architecture

```text
                    ┌──────────────┐
                    │   Incoming   │
                    │ Order/Cancel │
                    └──────┬───────┘
                           │
                           ▼
                    ┌──────────────┐
                    │  OrderBook   │
                    └──────┬───────┘
                           │
              ┌────────────┴────────────┐
              ▼                         ▼
       ┌─────────────┐           ┌─────────────┐
       │ Buy Levels  │           │ Sell Levels │
       │  High → Low │           │  Low → High │
       └──────┬──────┘           └──────┬──────┘
              │                         │
              └────────────┬────────────┘
                           │
                           ▼
              ┌────────────────────────┐
              │ OrderPool + OrderIndex │
              │    (FIFO per price)    │
              └────────────┬───────────┘
                           │ trades
                           ▼
                    ┌──────────────┐
                    │  TradeSink   │
                    └──────┬───────┘
                           │
                           ▼
              ┌────────────────────────┐
              │    AsyncTradeSink      │
              │  SPSC queue + thread   │
              └────────────────────────┘
```

`PriceLevels` keeps a dense window of price levels backed by word-level and bit-level occupancy bitmaps, so the best price can be found without scanning every level. Prices outside the dense window are stored in ordered maps. Resting orders live in an `OrderPool`; an `OrderIndex` maps order IDs to pool slots for FIFO maintenance and cancellation.

Matching submits fills through a non-owning `TradeSink` reference. Benchmarks use `AsyncTradeSink`, which enqueues trades on a lock-free SPSC ring buffer and drains them on a dedicated thread. Tests can use `havarti::support::MockTradeSink` to capture trades synchronously.

## Benchmarks

Benchmarks were run on Apple Silicon using a native optimized build (`-O3 -mcpu=native`) with a single benchmark thread.

| Metric      |               Result |
| ----------- | -------------------: |
| Throughput  | **36.0M orders/sec** |
| p50 latency |            **32 ns** |
| p99 latency |            **58 ns** |

The latency benchmark processes orders in batches of 100 and reports the average processing time per order for each batch. Batching is used because the effective clock granularity on the test system was too coarse to reliably measure individual orders at nanosecond-scale latencies. The latency benchmark also reports p99.9 and p99.99 percentiles and trade-count statistics per order.

On macOS, benchmarks pin themselves to performance cores when available.

Results may vary with CPU scheduling and system load.

## Building

Requires a C++20 compiler and CMake 3.16+.

```bash
./scripts/build.sh debug
```

For optimized builds:

```bash
./scripts/build.sh release
./scripts/build.sh release-native
```

`release-native` enables `-O3` and `-mcpu=native`.

GoogleTest-based tests are built only when using the `debug` configuration (`CMAKE_BUILD_TYPE=Debug`).

## Running

Run the test suite with:

```bash
./scripts/run_tests.sh
```

(`run_tests.sh` expects a prior `./scripts/build.sh debug`.)

Run the benchmark suite with:

```bash
./scripts/run_benchmarks.sh
```

Available benchmarks include:

* `bench_throughput` — sustained order processing
* `bench_latency` — per-batch order processing latency
* `bench_deep_book` — matching against a heavily populated book
* `bench_one_sided_pressure` — sustained imbalance between buys and sells
* `bench_dense_price_levels` — performance within the dense price-level window

## Project Structure

```text
havarti/
├── include/        # Public headers and core data structures
├── src/            # Core implementation
├── support/        # Shared development and test support
├── bench/          # Benchmark implementations and infrastructure
├── tests/          # Test case implementations and infrastructure
├── scripts/        # Build and benchmark scripts
└── CMakeLists.txt
```

Core matching lives in the `havarti_core` library; benchmarks and tests link against `havarti_support`, which provides the order generator and test helpers.

## Status

Havarti is a performance-oriented experimental matching engine. The core matching, price-level infrastructure, order cancellation, and async trade delivery path are implemented. Advanced order types, event replay, concurrency, and production-grade error handling remain future work.
