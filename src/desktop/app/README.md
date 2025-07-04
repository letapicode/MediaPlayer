# Desktop Qt Application

## Build Prerequisites
- Qt 6 (tested with 6.5)
- CMake 3.16+

## Building
```
mkdir build && cd build
cmake ../src/desktop/app
cmake --build .
```

Run the resulting `mediaplayer_desktop_app` binary from the build directory.

Packaging scripts are available under `installers/` for Windows (PowerShell + NSIS), macOS (bash + `macdeployqt`) and Linux (AppImage/deb via `linuxdeployqt`).

## Features
- Library and playlist browsing
- Visualization and video playback widgets
- Cross device sync prototype
- Basic queue view via `NowPlayingView`

The application loads translations based on your system locale. English and Spanish `.qm` files are included in the `translations` directory.
