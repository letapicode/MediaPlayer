# MediaPlayer

MediaPlayer is an open-source, cross-platform player focused on speed and a polished user experience.

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
# Example build steps (conceptual)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Usage

Run the resulting `mediaplayer` executable with a media file:

```
./mediaplayer path/to/video.mp4
```

The graphical interface allows you to open files and manage your library as features are implemented.
