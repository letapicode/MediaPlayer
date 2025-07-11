#!/bin/bash
# Run memory and CPU profiling on MediaPlayer.
# Usage: ./memory_cpu_profile.sh <media_file>
set -e
if [ $# -lt 1 ]; then
  echo "Usage: $0 <media_file>" >&2
  exit 1
fi
MEDIA=$1
ROOT=$(dirname "$(dirname "$0")")
TOOLS="$ROOT/../tools"
EXEC=mediaplayer_desktop_app
if [ ! -x "$EXEC" ]; then
  echo "$EXEC binary not found. Build the project first." >&2
  exit 1
fi
$TOOLS/memory_profile.sh ./$EXEC "$MEDIA"
$TOOLS/cpu_profile.sh ./$EXEC "$MEDIA"
