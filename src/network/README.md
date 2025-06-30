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

## Protocol Support

`ProtocolSupport` provides helpers to query supported input protocols at runtime.

```cpp
for (const auto &proto : mediaplayer::inputProtocols())
  std::cout << proto << std::endl;
```

## Internet Radio

`InternetRadio` extends `NetworkStream` and enables ICY metadata for HTTP radio
streams. Call `currentTitle()` to retrieve the `StreamTitle` if available.

## YouTube Helper

`YoutubeDL` wraps the `youtube-dl` tool to resolve streaming URLs.
