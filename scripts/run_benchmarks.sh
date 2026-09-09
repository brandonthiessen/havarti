#!/usr/bin/env bash

ROOT_DIR="$(dirname "$0")/.."

if [ -d "$ROOT_DIR/build/release-native/bench" ]; then
    BUILD_DIR="$ROOT_DIR/build/release-native/bench"
elif [ -d "$ROOT_DIR/build/release/bench" ]; then
    BUILD_DIR="$ROOT_DIR/build/release/bench"
else
    echo "Error: neither build/release-native/bench nor build/release/bench exists."
    exit 1
fi

echo "Build directory: $BUILD_DIR"
echo

for bench in "$BUILD_DIR"/bench_*; do
    [ -x "$bench" ] && "$bench"
done
