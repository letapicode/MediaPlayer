# MediaPlayer

MediaPlayer is an open-source, cross-platform AI-enabled media player focused on speed and a polished user experience.
High level design is described in [Masterplan.MD](Masterplan.MD) and a breakdown of work items is in [Tasks.MD](Tasks.MD).

See `docs/` for additional documentation and `src/` for module code.

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

```
# Example build steps
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

## Contributing

Please read our [Code of Conduct](CODE_OF_CONDUCT.md) before participating.
