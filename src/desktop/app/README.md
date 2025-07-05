# Desktop Qt Application

## Build Prerequisites
- Qt 6 (tested with 6.5)
- CMake 3.16+

## Building
Run CMake from the repository root so all modules are configured:
```
mkdir build && cd build
cmake ..
cmake --build . --target mediaplayer_desktop_app
```

CMake automatically compiles the `.ts` translation files found in
`translations/` and places the resulting `.qm` files in the build directory.

Run the resulting `mediaplayer_desktop_app` binary from the build directory.

Packaging scripts are available under `installers/` for Windows (PowerShell + NSIS), macOS (bash + `macdeployqt`) and Linux (AppImage/deb via `linuxdeployqt`).

## Packaging
The `installers` directory contains helper scripts to produce distributable packages.

- `installers/windows/package.ps1` – Runs `windeployqt` and builds the NSIS installer. Set `BUILD_DIR` to your build output directory before running.
- `installers/macos/package.sh` – Uses `macdeployqt` to bundle Qt frameworks then creates a DMG. Requires `BUILD_DIR` pointing at the build folder.
- `installers/linux/build_appimage.sh` – Invokes `linuxdeployqt` to create an AppImage or `.deb`. Set `BUILD_DIR` accordingly and pass `appimage` or `deb` as the first argument.

Ensure `windeployqt`, `macdeployqt` or `linuxdeployqt` are available in your `PATH` depending on platform.

Example commands run from the repository root:

```powershell
# Windows
$env:BUILD_DIR="build\Release"
./src/desktop/app/installers/windows/package.ps1
```

```bash
# macOS
BUILD_DIR=build ./src/desktop/app/installers/macos/package.sh

# Linux (AppImage)
BUILD_DIR=build ./src/desktop/app/installers/linux/build_appimage.sh appimage
```

## Features
- Library and playlist browsing
- Visualization and video playback widgets
- Cross device sync prototype
- Basic queue view via `NowPlayingView`

The application loads translations based on your system locale. English and Spanish `.qm` files are included in the `translations` directory.
