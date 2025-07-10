#!/usr/bin/env python3
"""Check that MediaPlayer can open various media formats."""
import subprocess
import sys
from pathlib import Path

PROBE = Path(__file__).resolve().parent / 'format_probe'
SAMPLES = Path(__file__).resolve().parent / 'sample_media'


def main():
    if not PROBE.exists():
        print('format_probe binary not built')
        return 1
    if not SAMPLES.exists():
        print('sample_media directory missing')
        return 1
    failed = []
    for media in SAMPLES.iterdir():
        if media.suffix.lower() not in {'.mp3', '.flac', '.wav', '.mp4', '.mkv', '.avi'}:
            continue
        print(f'Testing {media.name}...', end=' ', flush=True)
        result = subprocess.run([str(PROBE), str(media)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if result.returncode == 0:
            print('ok')
        else:
            print('fail')
            failed.append(media.name)
    if failed:
        print('\nFailed formats:', ', '.join(failed))
        return 1
    print('\nAll formats opened successfully')
    return 0


if __name__ == '__main__':
    sys.exit(main())
