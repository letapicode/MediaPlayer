import tempfile
import os
from itertools import count
from typing import Dict, Any

from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.responses import JSONResponse

from genre_classifier import classify_genre
from mood_detector import detect_mood
from speech_to_text import transcribe_audio
from object_scene_detector import detect_objects
from face_recognition import recognize_faces
from scene_segmentation import segment_scenes
from fingerprinter import fingerprint
from tag_worker import TagWorker

app = FastAPI()
worker = TagWorker()
_results: Dict[int, Any] = {}
_job_ids = count(1)


def _enqueue_job(path: str, handler) -> int:
    job_id = next(_job_ids)

    def _callback(result):
        _results[job_id] = result
        os.unlink(path)

    worker.enqueue(path, handler, _callback)
    return job_id


def _save_upload(upload: UploadFile) -> str:
    """Persist uploaded file and return the temporary path."""
    suffix = os.path.splitext(upload.filename)[1]
    try:
        fd, path = tempfile.mkstemp(suffix=suffix)
        with os.fdopen(fd, "wb") as f:
            f.write(upload.file.read())
        return path
    except Exception as exc:
        if 'fd' in locals():
            os.close(fd)
            os.unlink(path)
        raise HTTPException(status_code=500, detail="Failed to save upload") from exc


def _audio_handler(path: str) -> Dict[str, Any]:
    return {
        "genre": classify_genre(path),
        "mood": detect_mood(path),
        "fingerprint": fingerprint(path),
        "transcript": transcribe_audio(path),
    }


@app.post("/tag/audio")
async def tag_audio(file: UploadFile = File(...)):
    path = _save_upload(file)
    job_id = _enqueue_job(path, _audio_handler)
    return {"job_id": job_id}


def _video_handler(path: str) -> Dict[str, Any]:
    return {
        "objects": detect_objects(path),
        "faces": recognize_faces(path),
        "scenes": segment_scenes(path),
        "transcript": transcribe_audio(path),
    }


@app.post("/tag/video")
async def tag_video(file: UploadFile = File(...)):
    path = _save_upload(file)
    job_id = _enqueue_job(path, _video_handler)
    return {"job_id": job_id}


@app.get("/result/{job_id}")
async def get_result(job_id: int):
    if job_id in _results:
        return JSONResponse(_results.pop(job_id))
    return JSONResponse({"status": "pending"})


if __name__ == "__main__":
    import uvicorn
    # Start the service with ``python service_api.py`` or using uvicorn
    # directly: ``uvicorn service_api:app``.
    uvicorn.run(app, host="0.0.0.0", port=8000)
