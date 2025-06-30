# Streaming & Networking

This module contains basic networking helpers.

The `NetworkStream` class wraps FFmpeg to open media from HTTP/HTTPS URLs.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```

The `HlsStream` helper selects the highest quality variant from an HLS playlist.

```cpp
mediaplayer::HlsStream hls;
if (hls.open("https://example.com/live/stream.m3u8")) {
  AVFormatContext *ctx = hls.context();
  // use ctx for playback
}
```
