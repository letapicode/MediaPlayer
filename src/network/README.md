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

The helper function `protocolAvailable` can be used to verify that a protocol
such as `"http"` or `"rtmp"` is supported by the FFmpeg build:

```cpp
if (mediaplayer::protocolAvailable("http")) {
  // safe to open HTTP URLs
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

Using `DashStream`:

```cpp
mediaplayer::DashStream dash;
if (dash.open("https://example.com/manifest.mpd")) {
  AVFormatContext *ctx = dash.context();
  // ctx contains the selected audio and video streams
}
```

## InternetRadioStream

`InternetRadioStream` uses libcurl to read ICY metadata from internet radio
streams. When a `StreamTitle` change is detected, the provided callback is
invoked with the new title.

```cpp
mediaplayer::InternetRadioStream radio;
radio.setMetadataCallback([](const std::string &title) {
  std::cout << "Now playing: " << title << std::endl;
});
radio.open("http://example.com/stream");
```
