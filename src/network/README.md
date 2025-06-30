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

`HlsStream` and `DashStream` handle adaptive streaming playlists:

```cpp
mediaplayer::HlsStream hls;
if (hls.open("https://test-streams.mux.dev/x36xhzz/x36xhzz.m3u8")) {
  AVFormatContext *ctx = hls.context();
  // use the context
}

mediaplayer::DashStream dash;
dash.open("https://example.com/manifest.mpd", /*variant index*/ 0);
```
