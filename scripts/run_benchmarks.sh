#!/usr/bin/env bash

ROOT_DIR="$(dirname "$0")/.."

if [ -d "$ROOT_DIR/build/release-native" ]; then
    BUILD_DIR="$ROOT_DIR/build/release-native"
elif [ -d "$ROOT_DIR/build/release" ]; then
    BUILD_DIR="$ROOT_DIR/build/release"
else
    echo "Error: neither build/release-native nor build/release exists."
    exit 1
fi

echo "Build directory: $BUILD_DIR"
echo

for bench in "$BUILD_DIR"/bench_*; do
    [ -x "$bench" ] && "$bench"
done
