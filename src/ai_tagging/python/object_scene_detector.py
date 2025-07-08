from pathlib import Path
from typing import List

import cv2
import numpy as np
import onnxruntime as ort

MODEL_PATH = Path(__file__).parent / "models" / "object_detector.onnx"
LABELS_PATH = Path(__file__).parent / "models" / "object_labels.txt"

_session = ort.InferenceSession(str(MODEL_PATH))
_labels = []
if LABELS_PATH.exists():
    with open(LABELS_PATH) as f:
        _labels = [l.strip() for l in f]


def detect_objects(path: str, interval: float = 2.0) -> List[str]:
    cap = cv2.VideoCapture(path)
    if not cap.isOpened():
        return []
    tags = set()
    fps = cap.get(cv2.CAP_PROP_FPS) or 30.0
    step = int(fps * interval)
    frame_idx = 0
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        if frame_idx % step == 0:
            img = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            img = cv2.resize(img, (224, 224))
            data = img.astype(np.float32)[None].transpose(0, 3, 1, 2)
            out = _session.run(None, { _session.get_inputs()[0].name: data })[0]
            label_idx = int(np.argmax(out))
            if _labels:
                tags.add(_labels[label_idx])
            else:
                tags.add(str(label_idx))
        frame_idx += 1
    cap.release()
    return list(tags)
