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

## YouTubeDL Helper

`YouTubeDL` provides a simple way to resolve YouTube links to direct stream URLs using the external `youtube-dl` tool.

```cpp
std::string direct;
if (mediaplayer::YouTubeDL::getStreamUrl("https://youtube.com/watch?v=...", direct)) {
  // open `direct` with MediaDemuxer
}
```

The command `youtube-dl -g` must be available in the PATH (installable via `pip install youtube_dl`).

## DLNA/UPnP Client

`DlnaClient` discovers local DLNA servers and lists media:

```cpp
mediaplayer::upnp::DlnaClient client;
auto devices = client.discover();
for (const auto &d : devices) {
    auto items = client.listMedia(d);
    for (const auto &t : items)
        std::cout << t << std::endl;
}
```

## Remote Control HTTP Server

`RemoteControlServer` exposes `/status` and `/play` endpoints.
Use `RemoteControlClient` to interact with another device.

```cpp
mediaplayer::remote::RemoteControlServer server;
server.start();
server.setStatus({"Desktop", currentPath, position});

mediaplayer::remote::RemoteControlClient rc;
auto stat = rc.getStatus("192.168.1.5", 56791);
rc.sendPlay("192.168.1.5", 56791, "/path/media.mp4", 10.0);
```
