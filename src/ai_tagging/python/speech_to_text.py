"""Speech-to-text utilities using a HuggingFace model."""

from pathlib import Path

from transformers import pipeline

_MODEL_DIR = Path(__file__).parent / "models" / "speech"
_pipeline = None


def _get_pipeline():
    global _pipeline
    if _pipeline is None:
        _pipeline = pipeline(
            "automatic-speech-recognition",
            model=str(_MODEL_DIR),
            tokenizer=str(_MODEL_DIR),
        )
    return _pipeline


def transcribe_audio(path: str) -> str:
    """Return a transcript string for the given audio file."""
    asr = _get_pipeline()
    return asr(path)["text"]
