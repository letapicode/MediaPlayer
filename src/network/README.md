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

The `InternetRadioStream` class works similarly but also reports ICY metadata
when available.

```cpp
mediaplayer::InternetRadioStream radio;
radio.setMetadataCallback([](const std::string &title) {
  std::cout << "Now playing: " << title << '\n';
});
if (radio.open("http://example.com/stream")) {
  AVFormatContext *ctx = radio.context();
  // feed ctx to MediaPlayer
}
```
