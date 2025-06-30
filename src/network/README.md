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
