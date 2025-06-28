# Core Media Engine

This directory contains the cross‑platform playback engine built with CMake.

```
mkdir build && cd build
cmake ..
make
```

The resulting `mediaplayer_core` library provides working demuxing and decoding
using FFmpeg. `MediaDemuxer` opens media files or network streams and
enumerates the available audio, video and subtitle tracks. `AudioDecoder` and
`VideoDecoder` convert packets to raw PCM samples and RGBA frames. Hardware
acceleration can be enabled when supported by the platform and FFmpeg build.

Demuxing and decoding run on dedicated threads. Packet queues buffer data to
keep playback smooth even when reading from slow sources.
