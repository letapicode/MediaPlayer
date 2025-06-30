#!/usr/bin/env python3
"""Resolve a YouTube URL to a direct media stream using youtube_dl."""
import sys
from youtube_dl import YoutubeDL


def main():
    if len(sys.argv) != 2:
        print("Usage: youtube_resolve.py <URL>")
        return
    url = sys.argv[1]
    ydl_opts = {"quiet": True, "skip_download": True, "format": "best"}
    with YoutubeDL(ydl_opts) as ydl:
        info = ydl.extract_info(url, download=False)
        print(info.get("url", ""))


if __name__ == "__main__":
    main()
