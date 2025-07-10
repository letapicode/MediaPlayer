#!/usr/bin/env python3
"""End-to-end automation example for MediaPlayer.

The script demonstrates how to drive the application either on the desktop
using Qt's `QTest` module or on Android using `adb` commands.

Desktop mode loads `Main.qml` with PyQt5 and sends QTest events to scan a
folder, play a file, seek forward and pause. Android mode issues the
corresponding `adb shell` commands to an installed application.

Usage:
    python3 end_to_end_automation.py desktop <path-to-Main.qml> <media-folder>
    python3 end_to_end_automation.py android <package.name> <media-folder>

The script is intentionally simple and may require adaptation for a real
continuous integration environment.
"""

import os
import sys
import subprocess
import time


def run_desktop(qml_path: str, media_dir: str) -> None:
    """Automate basic playback on a desktop build using PyQt5."""
    from PyQt5.QtCore import QUrl, Qt
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtQml import QQmlApplicationEngine
    from PyQt5.QtTest import QTest

    app = QApplication([])
    engine = QQmlApplicationEngine()
    engine.load(QUrl.fromLocalFile(qml_path))
    if not engine.rootObjects():
        raise RuntimeError("failed to load UI")

    root = engine.rootObjects()[0]
    ctx = engine.rootContext()
    library = ctx.contextProperty("libraryQt")
    player = ctx.contextProperty("player")
    if library is None or player is None:
        raise RuntimeError("context properties missing")

    library.startScan(media_dir)
    QTest.qWait(3000)  # wait for scan to pick up files

    files = [f for f in os.listdir(media_dir) if f.lower().endswith((".mp3", ".wav", ".mp4"))]
    if not files:
        raise RuntimeError("no media files in folder")
    path = os.path.join(media_dir, files[0])

    player.openFile(path)
    player.play()
    QTest.qWait(1000)

    player.seek(5)
    QTest.qWait(500)
    player.pause()

    print("Desktop automation finished")


def run_android(package: str, media_dir: str) -> None:
    """Automate playback on Android using adb."""
    subprocess.run([
        "adb",
        "shell",
        "am",
        "broadcast",
        "-a",
        f"{package}.SCAN",
        "--es",
        "folder",
        media_dir,
    ], check=False)

    subprocess.run(["adb", "shell", "am", "start", "-n", f"{package}/.MainActivity"], check=False)
    time.sleep(3)

    subprocess.run(["adb", "shell", "input", "keyevent", "KEYCODE_MEDIA_PLAY_PAUSE"], check=False)
    time.sleep(1)
    subprocess.run(["adb", "shell", "input", "keyevent", "KEYCODE_MEDIA_FAST_FORWARD"], check=False)
    time.sleep(1)
    subprocess.run(["adb", "shell", "input", "keyevent", "KEYCODE_MEDIA_PLAY_PAUSE"], check=False)

    print("Android automation finished")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print(
            "Usage: python3 end_to_end_automation.py <desktop|android> <qml or package> <media dir>"
        )
        sys.exit(1)

    mode = sys.argv[1]
    arg = sys.argv[2]
    folder = sys.argv[3]

    if mode == "desktop":
        run_desktop(arg, folder)
    else:
        run_android(arg, folder)
