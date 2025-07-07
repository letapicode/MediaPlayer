#!/bin/bash
# Simple helper to build the iOS app via xcodebuild
set -e
DIR=$(cd "$(dirname "$0")/.." && pwd)
PROJECT="$DIR/src/ios/app/MediaPlayerApp.xcodeproj"
SCHEME="MediaPlayerApp"

if [ ! -d "$PROJECT" ]; then
  echo "Xcode project not found at $PROJECT" >&2
  exit 1
fi

xcodebuild -project "$PROJECT" -scheme "$SCHEME" -configuration Release build

