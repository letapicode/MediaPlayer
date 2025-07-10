#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

BIN_DIR = Path(__file__).resolve().parent

TESTS = [
    'format_converter_test',
    'library_db_test',
    'visualizer_test',
    'playback_integration_test',
    'startup_time_test'
]


def main():
    for t in TESTS:
        exe = BIN_DIR / t
        if not exe.exists():
            print(f'Skip {t} (not built)')
            continue
        print(f'Running {t}...')
        subprocess.run([str(exe)], check=True)
    print('End-to-end script finished')


if __name__ == '__main__':
    sys.exit(main())
