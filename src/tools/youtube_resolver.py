import sys
from youtube_dl import YoutubeDL

def main():
    if len(sys.argv) != 2:
        print("Usage: youtube_resolver.py <youtube_url>", file=sys.stderr)
        return 1
    url = sys.argv[1]
    opts = {
        'quiet': True,
        'no_warnings': True,
        'skip_download': True,
        'format': 'best'
    }
    try:
        with YoutubeDL(opts) as ydl:
            info = ydl.extract_info(url, download=False)
            stream_url = info.get('url')
            if not stream_url:
                formats = info.get('formats') or []
                if formats:
                    stream_url = formats[0].get('url')
            if stream_url:
                print(stream_url)
                return 0
    except Exception as e:
        print(f"Error resolving YouTube URL: {e}", file=sys.stderr)
    return 1

if __name__ == '__main__':
    sys.exit(main())
