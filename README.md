#MediaPlayer

MediaPlayer is an open-source, cross-platform AI-enabled media player focused on speed and a polished user experience.
High level design is described in [Masterplan.MD](Masterplan.MD) and a breakdown of work items is in [Tasks.MD](Tasks.MD).
For a quick index of task IDs see [parallel_tasks.md](parallel_tasks.md).
If you modify `Tasks.MD`, regenerate that table with `python tools/generate_parallel_tasks.py > parallel_tasks.md`.

See `docs/` for additional documentation and `src/` for module code. The
new [library integration guide](docs/library_integration.md) describes how the
SQLite library ties into the core engine and UI.

## Project Goals

The [Masterplan](Masterplan.MD) outlines an ambitious roadmap. In summary, the project aims to:

- Run on Windows, macOS, Linux, Android and iOS.
- Offer features such as smart playlists, real-time visualizations, gesture/voice control, AI-powered tagging, format conversion, streaming service integration, subtitle synchronization and optional cloud sync.
- Use a performance-first design built with C++17, FFmpeg and Qt, utilizing hardware acceleration when possible.
- Deliver an elegant UI that goes beyond existing media players.

A detailed breakdown of planned modules and tasks can be found in [Tasks.MD](Tasks.MD).

## Build Prerequisites

- Compiler with C++17 support
- FFmpeg development libraries
- Qt 6 development files
- OpenGL headers and GLFW 3
- SQLite3 built with FTS5 support
- projectM development headers (`libprojectM-dev`)

See [docs/building.md](docs/building.md) for a complete build walkthrough,
including how to compile the sample test programs.

```
#Example build steps
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

This will build the `mediaplayer_core` library defined in `src/core`,
`mediaplayer_subtitles` for SRT parsing and the
`mediaplayer_conversion` library in `src/format_conversion`.
The core library links with FFmpeg and can open media files using
`avformat_open_input`. The conversion module provides a simple API to
convert audio files between formats while the subtitles module parses
SRT files.
The SQLite-based library tracks media metadata, including an optional rating
field, and updates play counts when items are played through the core engine.
Full text search is powered by SQLite's FTS5 module, so ensure your SQLite
installation includes this extension.
Use `LibraryDB::searchFts()` for token-based queries or `search()` for simple
substring matching.
Smart playlists are defined using the same filter syntax as `LibraryDB::smartQuery`,
so filters like `rating>=4 AND artist='Queen'` work for both queries and playlists.

## Hardware Decoding

Hardware accelerated video decoding is supported when FFmpeg is built with the
appropriate backends. Use `MediaPlayer::setPreferredHardwareDevice()` to select
the device at runtime. Pass `-DENABLE_HW_DECODING=OFF` to CMake to force
software decoding.
Supported device names are `dxva2`, `d3d11va`, `videotoolbox`, `vaapi` and
`mediacodec` depending on the platform.

- **Windows**: DXVA2 or D3D11
- **macOS/iOS**: VideoToolbox
- **Linux**: VAAPI
- **Android**: MediaCodec (via JNI)

## Audio Visualization

The visualization module uses the [projectM](https://github.com/projectM-visualizer/projectm)
library to render classic MilkDrop presets in real time. `ProjectMVisualizer` wraps
libprojectM and produces an OpenGL texture that can be displayed in the UI. Attach
an instance with `MediaPlayer::setVisualizer()` and toggle output with
`enableVisualization(true)`. The Qt `VisualizerQt` helper exposes the texture to
QML and `VisualizerItem` renders it on screen. Adjust the mesh size, frame rate
or preset directory via the `ProjectMVisualizer::Config` struct to tune performance
on low power devices.

## Continuous Integration

All pushes and pull requests are built using GitHub Actions. The workflow
config in `.github/workflows/build.yml` compiles the project on Linux,
Windows and macOS with CMake to ensure the code builds across platforms.
The CI defines `SKIP_OFFLINE_TESTS=true` so optional tests that require
network access won't run in restricted environments.

## Usage

Run the resulting `mediaplayer` executable with a media file:

```
./mediaplayer path/to/video.mp4
```

The graphical interface allows you to open files and manage your library as features are implemented.

### Network Streams

MediaPlayer can open HTTP/HTTPS URLs and playlist formats like HLS (`.m3u8`) and
DASH (`.mpd`). The `mediaplayer_network` module wraps FFmpeg's networking
support and provides helpers for internet radio metadata and resolving YouTube
links via `youtube-dl`. See `src/network/README.md` for examples.

## Contributing

Please read our [Code of Conduct](CODE_OF_CONDUCT.md) before participating.

### Languages
The desktop UI currently ships English and Spanish translations. The application
selects the language based on your system locale at startup.

### Assets
Binary assets such as images are not stored in the repository. Instead reference the required asset or describe it so maintainers can add the file.
