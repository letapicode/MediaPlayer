#Building MediaPlayer

This guide explains how to compile the library modules and run the sample test programs under `tests/`.

## Prerequisites

Ensure the following packages are installed:

- **CMake** 3.15 or newer
- A C++17 compatible compiler
- **FFmpeg** development libraries
- **libcurl** development headers (`libcurl4-openssl-dev`) so FFmpeg supports HTTP/HTTPS streaming
- **TagLib** development headers
- **PulseAudio** (`libpulse` and `libpulse-simple`)
- **SQLite** development library
- Optional: Qt 6, OpenGL and GLFW for the desktop UI

On Ubuntu the packages can be installed with:

```bash
sudo apt-get install -y build-essential cmake git \
    libavcodec-dev libavformat-dev libswresample-dev libswscale-dev \
    libcurl4-openssl-dev \
    libsqlite3-dev libtag1-dev libpulse-dev libpulse-simple-dev
```

## Audio Output Backends

MediaPlayer includes several platform specific audio modules. The correct
backend is compiled automatically based on the target system, but
additional SDKs or packages are required for each one.

### Linux – PulseAudio

- Packages: `libpulse-dev` and `libpulse-simple-dev`
- CMake builds `AudioOutputPulse` when `CMAKE_SYSTEM_NAME` is `Linux`
- Runtime variables like `PULSE_SERVER` can be used to select a remote
  server

### Windows – WASAPI

- Requires the Windows SDK available with Visual Studio
- The WASAPI backend is chosen automatically when building with MSVC

### macOS – CoreAudio

- Requires Xcode and the macOS SDK providing `AudioToolbox` and
  `CoreAudio` frameworks
- No extra flags are needed; the backend is built on macOS

### Android – OpenSL ES / AAudio

- Requires the Android NDK (`ANDROID_NDK_HOME` should point to the NDK
  root)
- `AudioOutputAndroid` is compiled when using the Android toolchain

### iOS – AVAudio

- Requires Xcode with the iOS SDK installed
- CMake selects `AudioOutputiOS` when `CMAKE_SYSTEM_NAME` is `iOS`

## Building the libraries

Create a build directory and run CMake:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target mediaplayer_core mediaplayer_library mediaplayer_subtitles mediaplayer_conversion
```

This produces static libraries for the core engine, media library, subtitle parser and format conversion utilities.

## Building the test executables

The test programs in `tests/` can also be built with CMake. Configure the project with testing enabled and build the desired targets:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build . --target test_srtparser format_conversion_test library_playlist_test \
    library_db_update_test library_playback_update_test library_rating_test \
    library_search_test library_video_metadata_test subtitle_provider_test \
    video_conversion_test
```

Each test target corresponds to a source file in the `tests/` directory.

## Running the tests

After building, run a test binary from the build directory, for example:

```bash
./test_srtparser
```

Successful execution prints a short confirmation message.

## Building the conversion utility

The `mediaconvert` command line program can be built alongside the libraries:

```bash
cmake --build . --target mediaconvert
```

Use it to convert audio or video files on the command line.
