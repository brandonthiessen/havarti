#!/usr/bin/env bash

set -e

CONFIG="${1:-debug}"
BUILD_DIR="build/$CONFIG"

case "$CONFIG" in
    debug)
        CMAKE_CONFIG="Debug"
        ;;
    release)
        CMAKE_CONFIG="Release"
        ;;
    release-native)
        CMAKE_CONFIG="ReleaseNative"
        ;;
    *)
        echo "Usage: $0 [debug|release|release-native]"
        exit 1
        ;;
esac

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$CMAKE_CONFIG" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

cmake --build "$BUILD_DIR"
