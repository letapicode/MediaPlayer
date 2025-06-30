# mediaconvert CLI

This simple command line tool demonstrates the format conversion utilities.
It allows converting audio or video files using the `FormatConverter` class.

```
mediaconvert <audio|video> <input> <output> [options]
```

Options include bitrate, codec and, for video, the width/height or `--crf` value
to control quality. Conversion progress is printed to the console.

## Playing a YouTube URL

`YoutubeResolver` uses a small Python helper to turn YouTube links into direct
stream URLs. The resolved URL can then be passed to `NetworkStream`:

```cpp
std::string direct = mediaplayer::YoutubeResolver::resolve(
    "https://www.youtube.com/watch?v=VIDEO_ID");
mediaplayer::NetworkStream stream;
if (stream.open(direct)) {
  // pass stream.context() to your player
}
```

