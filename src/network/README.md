# Streaming & Networking

This module contains basic networking helpers.

The `NetworkStream` class wraps FFmpeg to open media from HTTP/HTTPS URLs. It
initializes FFmpeg's network stack and can be used for HLS/DASH streams as well
when the corresponding demuxers are available in the local FFmpeg build.

`NetworkUtils` exposes helper functions to query supported protocols and input
formats. These are useful to verify that the FFmpeg build includes modules such
as `http`, `hls` or `dash`:

```cpp
for (const auto &p : mediaplayer::listProtocols())
  std::cout << p << "\n";
if (mediaplayer::supportsDemuxer("hls"))
  std::cout << "HLS supported" << std::endl;
```

`InternetRadioStream` extends `NetworkStream` and parses ICY metadata from audio
streams. The metadata can be refreshed at runtime to obtain the current song
title.

YouTube URLs can be resolved to direct media streams using the optional helper
`YoutubeDL` which calls `youtube_dl` via Python. Ensure `pip install youtube_dl`
is run so the Python module is available.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```
