# Havarti

Havarti is a high-performance limit order book and matching engine written in C++20.

It implements **price-time priority matching**, FIFO order queues, cache-friendly price-level indexing, and a lock-free SPSC trade pipeline. The project also includes a synthetic order generator and several benchmarks for measuring throughput and latency under different market conditions.

## Features

* Price-time priority matching
* FIFO queues at each price level
* Dense price-level indexing using bitmaps
* `std::map` fallback for prices outside the dense range
* Lock-free single-producer/single-consumer trade queue
* Deterministic synthetic order generation
* Throughput and latency benchmarks
* C++20 / CMake build system

## Architecture

```text
                    ┌──────────────┐
                    │   Incoming   │
                    │     Order    │
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
                    ┌──────────────┐
                    │  TradeSink   │
                    │  SPSC Queue  │
                    └──────────────┘
```

`PriceLevels` keeps a dense window of price levels backed by occupancy bitmaps, allowing the best price to be found without scanning every price level. Prices outside the dense window are stored in ordered maps.

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

## Running

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
├── src/            # Order book, price levels, trade sink, generator
├── bench/          # Benchmark implementations
├── scripts/        # Build and benchmark scripts
└── CMakeLists.txt
```

## Status

Havarti is a performance-oriented experimental matching engine. The core matching and price-level infrastructure is implemented; trade handling and some production concerns are intentionally left as future work.
