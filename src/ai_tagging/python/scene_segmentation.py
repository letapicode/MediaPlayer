from typing import List, Tuple
import cv2
import numpy as np


def segment_scenes(path: str, threshold: float = 30.0) -> List[Tuple[float, str]]:
    """Return a list of (timestamp, label) scene boundaries."""
    cap = cv2.VideoCapture(path)
    if not cap.isOpened():
        return []
    prev = None
    fps = cap.get(cv2.CAP_PROP_FPS) or 30.0
    ts = 0.0
    scenes = []
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        if prev is not None:
            diff = cv2.absdiff(prev, gray)
            score = np.mean(diff)
            if score > threshold:
                scenes.append((ts, "cut"))
        prev = gray
        ts += 1.0 / fps
    cap.release()
    return scenes
