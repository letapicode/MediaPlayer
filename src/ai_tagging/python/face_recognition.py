"""Simple face recognition against a local database.

The ``models/faces`` directory should contain reference images named after the
person they represent, e.g. ``alice.jpg``. The :func:`recognize_faces` function
returns the list of known faces detected in a video file.
"""

from pathlib import Path
from typing import List

import cv2
import numpy as np
import face_recognition as fr

DB_PATH = Path(__file__).parent / "models" / "faces"

_database = []
if DB_PATH.exists():
    for img_path in DB_PATH.glob("*.jpg"):
        img = fr.load_image_file(str(img_path))
        enc = fr.face_encodings(img)
        if enc:
            _database.append((img_path.stem, enc[0]))


def recognize_faces(path: str) -> List[str]:
    """Identify known faces appearing in ``path``.

    Parameters
    ----------
    path: str
        Path to a video file.

    Returns
    -------
    List[str]
        Names corresponding to images stored in ``models/faces``.
    """

    video = cv2.VideoCapture(path)
    if not video.isOpened():
        return []
    names = set()
    while True:
        ret, frame = video.read()
        if not ret:
            break
        rgb = frame[:, :, ::-1]
        locs = fr.face_locations(rgb)
        encs = fr.face_encodings(rgb, locs)
        for enc in encs:
            for name, ref in _database:
                match = fr.compare_faces([ref], enc)[0]
                if match:
                    names.add(name)
    video.release()
    return list(names)
