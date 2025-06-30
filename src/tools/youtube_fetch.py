#!/usr/bin/env python3
"""Fetch direct media URLs using youtube-dl/youtube-dlp.

Usage:
  youtube_fetch.py <youtube-url>
"""
import sys

try:
    from youtube_dl import YoutubeDL  # type: ignore
except Exception:
    print("youtube_dl module not installed. Run 'pip install youtube_dl'.")
    sys.exit(1)

if len(sys.argv) != 2:
    print(__doc__)
    sys.exit(1)

url = sys.argv[1]
opts = {
    'quiet': True,
    'skip_download': True,
    'format': 'best',
}
with YoutubeDL(opts) as ydl:
    info = ydl.extract_info(url, download=False)
    if 'url' in info:
        print(info['url'])
    elif 'entries' in info and info['entries']:
        print(info['entries'][0]['url'])
    else:
        print('Failed to retrieve stream URL')
