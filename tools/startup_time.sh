#!/bin/bash
# Build and measure startup time of the desktop app
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="${SCRIPT_DIR}/.."
BUILD_DIR="${REPO_ROOT}/build_startup"

cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release >/dev/null
cmake --build "$BUILD_DIR" --target mediaplayer_desktop_app >/dev/null

OUTPUT=$(MEASURE_STARTUP=1 "$BUILD_DIR/mediaplayer_desktop_app" 2>&1)
SECONDS_VAL=$(echo "$OUTPUT" | grep -Eo '[0-9]+(\.[0-9]+)?(?= seconds)' | head -n1)
echo "$OUTPUT" | grep 'Startup completed' || true
if [ -n "$SECONDS_VAL" ]; then
    if awk -v s="$SECONDS_VAL" 'BEGIN{exit s<2?0:1}'; then
        echo "Startup time ${SECONDS_VAL}s (PASS)"
    else
        echo "Startup time ${SECONDS_VAL}s exceeds threshold" >&2
        exit 1
    fi
else
    echo "Failed to measure startup time" >&2
    exit 1
fi
