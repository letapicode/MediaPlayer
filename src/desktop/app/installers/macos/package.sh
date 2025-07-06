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
CODE_SIGN_IDENTITY="${CODE_SIGN_IDENTITY:-}" 
ENTITLEMENTS_FILE="${ENTITLEMENTS_FILE:-}" 

mkdir -p "$DIST_DIR"

# Copy built .app bundle to distribution directory
cp -R "$APP_BUNDLE_SRC" "$APP_BUNDLE"

# Deploy Qt frameworks and QML files
macdeployqt "$APP_BUNDLE" -qmldir="${PROJECT_ROOT}/src/desktop/app/qml" -verbose=1
cp -R "${BUILD_DIR}/translations" "$APP_BUNDLE/Contents/MacOS/" 2>/dev/null || true

# Code sign the .app bundle if an identity is provided
if [ -n "$CODE_SIGN_IDENTITY" ]; then
    echo "Signing app bundle with '$CODE_SIGN_IDENTITY'"
    if [ -n "$ENTITLEMENTS_FILE" ]; then
        codesign --force --options runtime --deep --sign "$CODE_SIGN_IDENTITY" \
            --entitlements "$ENTITLEMENTS_FILE" "$APP_BUNDLE"
    else
        codesign --force --options runtime --deep --sign "$CODE_SIGN_IDENTITY" \
            "$APP_BUNDLE"
    fi
fi

# Create compressed DMG
hdiutil create "$DIST_DIR/$DMG_NAME" -volname "MediaPlayer" -srcfolder "$APP_BUNDLE" -ov -format UDZO

echo "DMG created at $DIST_DIR/$DMG_NAME"

if [ -n "$CODE_SIGN_IDENTITY" ]; then
    codesign --force --sign "$CODE_SIGN_IDENTITY" "$DIST_DIR/$DMG_NAME"
    echo "Signed DMG with '$CODE_SIGN_IDENTITY'"
fi
