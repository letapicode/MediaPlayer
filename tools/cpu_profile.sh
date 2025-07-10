#!/bin/bash
# Run Valgrind Callgrind to profile CPU usage of the given executable.
# Usage: ./cpu_profile.sh <executable> [args]
set -e
if [ $# -lt 1 ]; then
  echo "Usage: $0 <executable> [args]" >&2
  exit 1
fi
valgrind --tool=callgrind --callgrind-out-file=callgrind.out "$@"
echo "Callgrind results written to callgrind.out"
