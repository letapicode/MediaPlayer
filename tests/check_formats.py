#!/usr/bin/env python3
"""Attempt playback of sample files for each media format.

The script reads a list of format extensions from `formats_list.txt` and
for each format tries to open a corresponding sample file located in
`sample_media/` using `ffprobe`. Results are written to `format_check.log`.
"""

import subprocess
import sys
from pathlib import Path

FORMATS_FILE = Path(__file__).with_name("formats_list.txt")
MEDIA_DIR = Path(__file__).parent / "sample_media"
LOG_FILE = Path(__file__).with_name("format_check.log")


def check_format(ext: str) -> str:
    """Return status string for a single format."""
    sample_path = MEDIA_DIR / f"sample.{ext}"
    if not sample_path.exists():
        return f"{ext}: sample file not found"
    try:
        subprocess.run(
            ["ffprobe", "-v", "error", str(sample_path)],
            check=True,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
        )
        return f"{ext}: success"
    except FileNotFoundError:
        return "ffprobe executable not found"
    except subprocess.CalledProcessError as e:
        err = e.stderr.decode().strip().splitlines()[-1]
        return f"{ext}: fail - {err}"


def main() -> int:
    if not FORMATS_FILE.exists():
        print(f"Formats list {FORMATS_FILE} missing")
        return 1
    results = []
    for line in FORMATS_FILE.read_text().splitlines():
        ext = line.strip()
        if not ext:
            continue
        results.append(check_format(ext))
    LOG_FILE.write_text("\n".join(results) + "\n")
    for r in results:
        print(r)
    return 0


if __name__ == "__main__":
    sys.exit(main())
