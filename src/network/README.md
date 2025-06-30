# Streaming & Networking

This module contains basic networking helpers. FFmpeg handles the actual
protocol implementations; make sure it is built with network support and
linked against `libcurl` so that HTTP(S), HLS, DASH and RTMP streams work.
On Ubuntu systems install `ffmpeg` and `libcurl4-openssl-dev`:

```bash
sudo apt-get install ffmpeg libcurl4-openssl-dev
```

The `NetworkStream` class wraps FFmpeg to open media from HTTP/HTTPS URLs and
provides helpers to read ICY metadata from internet radio streams.

```cpp
mediaplayer::NetworkStream stream;
if (stream.open("https://example.com/video.mp4")) {
  AVFormatContext *ctx = stream.context();
  // pass ctx to MediaPlayer or custom processing
}
```

To retrieve radio metadata:

```cpp
std::cout << stream.icyTitle() << std::endl;
```

### YouTube links

`youtube_dl` can be used to resolve streaming URLs. The helper function
`youtubeDlUrl()` returns a direct URL:

```cpp
auto direct = mediaplayer::youtubeDlUrl("https://www.youtube.com/watch?v=...");
```
