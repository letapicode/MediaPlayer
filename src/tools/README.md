# mediaconvert CLI

This simple command line tool demonstrates the format conversion utilities.
It allows converting audio or video files using the `FormatConverter` class.

```
mediaconvert <audio|video> <input> <output> [options]
```

Options include bitrate, codec and, for video, the width/height or `--crf` value
to control quality. Conversion progress is printed to the console.

## youtube_fetch

`youtube_fetch.py` is a tiny helper script that uses `youtube-dl` (or
`youtube-dlp`) to resolve streaming URLs from services like YouTube. It
prints the direct media URL so the core player can open it with
`NetworkStream`.

```
./youtube_fetch.py "https://www.youtube.com/watch?v=..."
```

