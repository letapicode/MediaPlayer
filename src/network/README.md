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

To resolve a YouTube link before opening it with `NetworkStream`, use the
`YouTubeResolver` helper which internally runs `youtube-dl`:

```cpp
mediaplayer::YouTubeResolver yt;
std::string direct = yt.resolve("https://www.youtube.com/watch?v=dQw4w9WgXcQ");

mediaplayer::NetworkStream stream;
if (stream.open(direct)) {
  // pass stream.context() to the player
}
```
