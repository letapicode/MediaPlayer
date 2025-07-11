#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
BUILD_DIR="${ROOT_DIR}/build"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DBUILD_TESTS=ON "$@"
cmake --build "$BUILD_DIR"

pushd "$BUILD_DIR" >/dev/null
ctest --output-on-failure
popd >/dev/null

python -m unittest discover -s "$ROOT_DIR/tests" -p "*_test.py"

python "$SCRIPT_DIR/end_to_end.py"

