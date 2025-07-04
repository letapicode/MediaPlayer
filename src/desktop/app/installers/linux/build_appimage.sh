#!/bin/bash
# Build an AppImage or .deb package using linuxdeployqt.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/../../../../.."
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build}"
DIST_DIR="${PROJECT_ROOT}/dist"
APP_NAME="mediaplayer_desktop_app"

APPIMAGE_DIR="${DIST_DIR}/${APP_NAME}.AppDir"
mkdir -p "$APPIMAGE_DIR"
mkdir -p "$DIST_DIR"

# Copy built binaries and resources
cp "${BUILD_DIR}/${APP_NAME}" "$APPIMAGE_DIR/"
cp -r "${PROJECT_ROOT}/src/desktop/app/qml" "$APPIMAGE_DIR/" 2>/dev/null || true
cp -r "${PROJECT_ROOT}/src/desktop/app/translations" "$APPIMAGE_DIR/" 2>/dev/null || true

PACKAGE_TYPE="${1:-appimage}" # 'appimage' or 'deb'

if [ "$PACKAGE_TYPE" = "deb" ]; then
    linuxdeployqt "$APPIMAGE_DIR/${APP_NAME}" -deb -unsupported-allow-new-glibc
    mv ./*.deb "$DIST_DIR/" 2>/dev/null || true
else
    linuxdeployqt "$APPIMAGE_DIR/${APP_NAME}" -appimage -unsupported-allow-new-glibc
    mv ./*.AppImage "$DIST_DIR/MediaPlayer.AppImage" 2>/dev/null || true
fi

echo "Packages placed in $DIST_DIR"
