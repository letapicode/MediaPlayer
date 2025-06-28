# Core Media Engine

This directory contains the cross‑platform playback engine. The initial
implementation is a minimal skeleton built with CMake.

```
mkdir build && cd build
cmake ..
make
```

The resulting `mediaplayer_core` library is currently a stub and does not yet
perform any decoding.

`MediaDemuxer` uses FFmpeg to open media files and enumerate audio, video and
subtitle streams. Decoding and playback will be added in future modules.
