# Streaming & Networking

This module contains networking helpers and wrappers over FFmpeg's
protocol implementations. FFmpeg must be built with networking support
(HTTP/HTTPS, HLS, etc.) and the system needs `libcurl` development
headers available. The Dockerfile and build instructions include the
`libcurl4-openssl-dev` package to satisfy this dependency.

`NetworkStream` wraps FFmpeg to open media from remote URLs including
static files, HLS/DASH playlists and internet radio streams.
It automatically initializes FFmpeg's network layer and exposes
metadata such as ICY headers when available.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```

After opening a radio stream you can query metadata keys such as
`icy-title` using `getMetadata()`:

```cpp
std::string title = stream.getMetadata("icy-title");
```

For sites like YouTube you can use the helper script in `src/tools` to
resolve a direct streaming URL:

```
./youtube_fetch.py "https://www.youtube.com/watch?v=..."
```
