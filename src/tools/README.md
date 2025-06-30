# mediaconvert CLI

This simple command line tool demonstrates the format conversion utilities.
It allows converting audio or video files using the `FormatConverter` class.

```
mediaconvert <audio|video> <input> <output> [options]
```

Options include bitrate, codec and, for video, the width/height or `--crf` value
to control quality. Conversion progress is printed to the console.

## youtube\_dlp\_fetch.py

`youtube_dlp_fetch.py` is a helper script that resolves a YouTube (or similar)
link to its direct media stream URL using the `youtube_dl` Python library.

```bash
python3 youtube_dlp_fetch.py "https://youtu.be/example"
```

The script prints the stream URL, which can then be passed to `MediaPlayer`.

