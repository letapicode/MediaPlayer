#!/bin/bash
# Build an AppImage or .deb package using linuxdeployqt.
#
# Usage: BUILD_DIR=path/to/build ./build_appimage.sh [appimage|deb]
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="${SCRIPT_DIR}/../../../../.."
QML_DIR="${PROJECT_ROOT}/src/desktop/app/qml"
BUILD_DIR="${BUILD_DIR:-${PROJECT_ROOT}/build}"
DIST_DIR="${PROJECT_ROOT}/dist"
APP_NAME="mediaplayer_desktop_app"

APPIMAGE_DIR="${DIST_DIR}/${APP_NAME}.AppDir"
mkdir -p "$APPIMAGE_DIR"
mkdir -p "$DIST_DIR"

# Ensure linuxdeployqt is available
command -v linuxdeployqt >/dev/null || {
    echo "linuxdeployqt not found in PATH" >&2
    exit 1
}

# Copy built binaries and resources
cp "${BUILD_DIR}/${APP_NAME}" "$APPIMAGE_DIR/"
cp -r "${BUILD_DIR}/qml" "$APPIMAGE_DIR/" 2>/dev/null || true
cp -r "${BUILD_DIR}/translations" "$APPIMAGE_DIR/" 2>/dev/null || true

PACKAGE_TYPE="${1:-appimage}" # 'appimage' or 'deb'

if [ "$PACKAGE_TYPE" = "deb" ]; then
    linuxdeployqt "$APPIMAGE_DIR/${APP_NAME}" -qmldir="$QML_DIR" -deb -verbose=2 -unsupported-allow-new-glibc
    mv ./*.deb "$DIST_DIR/" 2>/dev/null || true
else
    linuxdeployqt "$APPIMAGE_DIR/${APP_NAME}" -qmldir="$QML_DIR" -appimage -verbose=2 -unsupported-allow-new-glibc
    mv ./*.AppImage "$DIST_DIR/MediaPlayer.AppImage" 2>/dev/null || true
fi

echo "Packages placed in $DIST_DIR"
