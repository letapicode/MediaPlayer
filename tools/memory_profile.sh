#!/bin/bash
# Run Valgrind Massif to profile memory usage of the given executable.
# Usage: ./memory_profile.sh <executable> [args]
set -e
if [ $# -lt 1 ]; then
  echo "Usage: $0 <executable> [args]" >&2
  exit 1
fi
valgrind --tool=massif --massif-out-file=massif.out "$@"
ms_print massif.out > massif.txt
echo "Massif results written to massif.txt"
