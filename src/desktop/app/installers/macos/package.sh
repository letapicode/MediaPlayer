#!/bin/bash
# Package the macOS application bundle and create a DMG.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/../../../../.."
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build}"
DIST_DIR="${PROJECT_ROOT}/dist"

APP_BUNDLE_SRC="${BUILD_DIR}/mediaplayer_desktop_app.app"
APP_BUNDLE="${DIST_DIR}/MediaPlayer.app"
DMG_NAME="MediaPlayer.dmg"

mkdir -p "$DIST_DIR"

# Copy built .app bundle to distribution directory
cp -R "$APP_BUNDLE_SRC" "$APP_BUNDLE"

# Deploy Qt frameworks and QML files
macdeployqt "$APP_BUNDLE" -qmldir="${PROJECT_ROOT}/src/desktop/app/qml" -verbose=1

# Create compressed DMG
hdiutil create "$DIST_DIR/$DMG_NAME" -volname "MediaPlayer" -srcfolder "$APP_BUNDLE" -ov -format UDZO

echo "DMG created at $DIST_DIR/$DMG_NAME"
