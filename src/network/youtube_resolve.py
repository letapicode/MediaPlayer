#!/usr/bin/env python3
"""Resolve the best stream URL for a YouTube video."""

import sys

# Try importing youtube_dl or youtube_dlp
try:
    import youtube_dl as ytdl
except ImportError:
    try:
        import youtube_dlp as ytdl
    except ImportError:  # Neither package installed
        sys.stderr.write("Error: youtube_dl or youtube_dlp must be installed.\n")
        sys.exit(1)


def main() -> None:
    if len(sys.argv) < 2:
        sys.stderr.write("Usage: youtube_resolve.py <YouTube URL>\n")
        sys.exit(1)

    url = sys.argv[1]

    opts = {"quiet": True, "skip_download": True, "format": "best"}
    with ytdl.YoutubeDL(opts) as ydl:
        info = ydl.extract_info(url, download=False)

    stream_url = info.get("url")
    if not stream_url:
        formats = info.get("formats") or []
        if formats:
            stream_url = formats[-1].get("url")

    if stream_url:
        print(stream_url)
    else:
        sys.stderr.write("Failed to resolve stream URL\n")
        sys.exit(1)


if __name__ == "__main__":
    main()
