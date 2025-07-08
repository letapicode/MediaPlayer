"""Music genre prediction using an ONNX model.

The ``classify_genre`` function expects the path to an audio file and returns a
list of the top three predicted genre labels.
"""

from pathlib import Path
from typing import List

import numpy as np
import onnxruntime as ort
import torchaudio

MODEL_PATH = Path(__file__).parent / "models" / "genre_classifier.onnx"
LABELS_PATH = Path(__file__).parent / "models" / "genre_labels.txt"

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


def classify_genre(path: str) -> List[str]:
    """Return the top three genre labels for ``path``.

    Parameters
    ----------
    path: str
        Path to an audio file readable by ``torchaudio``.

    Returns
    -------
    List[str]
        Ordered list of genre names with the most likely first.
    """
    waveform, sr = torchaudio.load(path)
    if waveform.dim() > 1:
        waveform = waveform.mean(0, keepdim=True)
    waveform = torchaudio.functional.resample(waveform, sr, 16000)
    inputs = waveform.numpy()
    ort_inputs = { _session_instance().get_inputs()[0].name: inputs }
    logits = _session_instance().run(None, ort_inputs)[0]
    scores = np.squeeze(logits)
    top = np.argsort(scores)[-3:][::-1]
    if _LABELS:
        return [_LABELS[i] for i in top]
    return [f"label_{i}" for i in top]
