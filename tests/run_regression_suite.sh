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

if command -v qmltestrunner >/dev/null 2>&1; then
    qmltestrunner "$ROOT_DIR/tests/ui_responsiveness.qml"
else
    echo "Warning: qmltestrunner not found; skipping UI responsiveness test" >&2
fi

python "$SCRIPT_DIR/end_to_end.py"

