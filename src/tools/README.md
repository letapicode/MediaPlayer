# mediaconvert CLI

This simple command line tool demonstrates the format conversion utilities.
It allows converting audio or video files using the `FormatConverter` class.

```
mediaconvert <audio|video> <input> <output> [options]
```

Options include bitrate, codec and, for video, the width/height or `--crf` value
to control quality. Conversion progress is printed to the console.

`youtube_resolve.py` is a small helper script that uses `youtube_dl` to resolve a
YouTube URL to a direct media stream URL:

```
python3 youtube_resolve.py https://youtube.com/watch?v=...
```

