# Streaming & Networking

This module contains basic networking helpers.

The `NetworkStream` class wraps FFmpeg to open media from HTTP/HTTPS URLs. ICY
metadata is requested automatically so internet radio streams expose song
information when available.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```

For YouTube links, the optional helper `fetchYouTubeStream()` uses
`youtube-dl` to resolve the direct media URL:

```cpp
auto url = mediaplayer::fetchYouTubeStream("https://youtu.be/VIDEO_ID");
if (url) {
  stream.open(*url);
}
```
