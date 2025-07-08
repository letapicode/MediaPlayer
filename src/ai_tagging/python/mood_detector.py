from pathlib import Path
from typing import List

import numpy as np
import onnxruntime as ort
import torchaudio

MODEL_PATH = Path(__file__).parent / "models" / "mood_detector.onnx"
LABELS_PATH = Path(__file__).parent / "models" / "mood_labels.txt"

_session = None


def _session_instance() -> ort.InferenceSession:
    global _session
    if _session is None:
        _session = ort.InferenceSession(str(MODEL_PATH))
    return _session


def _load_labels() -> List[str]:
    if LABELS_PATH.exists():
        with open(LABELS_PATH) as f:
            return [l.strip() for l in f]
    return []


_LABELS = _load_labels()


def detect_mood(path: str) -> List[str]:
    """Return top mood predictions for the given audio file."""
    waveform, sr = torchaudio.load(path)
    if waveform.dim() > 1:
        waveform = waveform.mean(0, keepdim=True)
    waveform = torchaudio.functional.resample(waveform, sr, 16000)
    inputs = waveform.numpy()
    ort_inputs = {_session_instance().get_inputs()[0].name: inputs}
    logits = _session_instance().run(None, ort_inputs)[0]
    scores = np.squeeze(logits)
    top = np.argsort(scores)[-3:][::-1]
    if _LABELS:
        return [_LABELS[i] for i in top]
    return [f"label_{i}" for i in top]
