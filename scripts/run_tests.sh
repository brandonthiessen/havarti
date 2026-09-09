#!/usr/bin/env bash

set -e

ROOT_DIR="$(dirname "$0")/.."

if [ -x "$ROOT_DIR/build/debug/tests/havarti_tests" ]; then
    TEST_BINARY="$ROOT_DIR/build/debug/tests/havarti_tests"
else
    echo "Error: test binary not found:"
    echo "  $ROOT_DIR/build/debug/tests/havarti_tests"
    echo "Run ./scripts/build.sh debug first."
    exit 1
fi

echo "Test binary: $TEST_BINARY"
echo

"$TEST_BINARY"
