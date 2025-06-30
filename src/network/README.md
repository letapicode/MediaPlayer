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

The helper function `protocolAvailable` can be used to verify that a protocol
such as `"http"` or `"rtmp"` is supported by the FFmpeg build:

```cpp
if (mediaplayer::protocolAvailable("http")) {
  // safe to open HTTP URLs
}
```
