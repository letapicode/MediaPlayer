#!/bin/bash
# Build the iOS application using xcodebuild.
# Requires Xcode command line tools installed.
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/.."

PROJECT="$PROJECT_ROOT/src/ios/MediaPlayerApp.xcodeproj"
SCHEME="MediaPlayerApp"

if [ ! -d "$PROJECT" ]; then
  echo "Xcode project not found: $PROJECT" >&2
  exit 1
fi

xcodebuild -project "$PROJECT" \
           -scheme "$SCHEME" \
           -configuration Release \
           -destination 'generic/platform=iOS' \
           build
