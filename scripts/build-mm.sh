#!/usr/bin/env bash
# Build the self-contained `mm` engine-core demo (Track A).
# Works on Linux and macOS (needs cmake, ninja-or-make, and a C++20 compiler).
set -euo pipefail
cd "$(dirname "$0")/.."

BUILD_DIR="${BUILD_DIR:-build}"
GENERATOR="${GENERATOR:-Ninja}"

echo "==> Configuring (${GENERATOR})"
cmake -S . -B "$BUILD_DIR" -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release

echo "==> Building"
cmake --build "$BUILD_DIR"

echo "==> Running self-test (mm --core)"
ctest --test-dir "$BUILD_DIR" --output-on-failure

echo "==> Engine-core self test"
"$BUILD_DIR/mm" --core

echo
echo "Done. Binary: $BUILD_DIR/mm"