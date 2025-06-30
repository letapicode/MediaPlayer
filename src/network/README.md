# Streaming & Networking

This module contains basic networking helpers.

The `NetworkStream` class wraps FFmpeg to open media from HTTP/HTTPS URLs.

`DashStream` builds on top of this to handle simple MPEG‑DASH manifests. It
downloads the MPD file, picks the first audio and video representations and then
opens the stream with FFmpeg's dash demuxer.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```

Using `DashStream`:

```cpp
mediaplayer::DashStream dash;
if (dash.open("https://example.com/manifest.mpd")) {
  AVFormatContext *ctx = dash.context();
  // ctx contains the selected audio and video streams
}
```
