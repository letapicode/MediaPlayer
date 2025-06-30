#!/usr/bin/env python3
import sys
import youtube_dl


def main():
    if len(sys.argv) < 2:
        print("usage: youtube_dlp_fetch.py URL", file=sys.stderr)
        return 1
    ydl = youtube_dl.YoutubeDL({'quiet': True, 'format': 'best'})
    info = ydl.extract_info(sys.argv[1], download=False)
    print(info['url'])


if __name__ == '__main__':
    main()
