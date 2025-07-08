import tempfile
import os
from fastapi import FastAPI, UploadFile, File
from fastapi.responses import JSONResponse
from genre_classifier import classify_genre
from mood_detector import detect_mood
from speech_to_text import transcribe_audio
from object_scene_detector import detect_objects
from face_recognition import recognize_faces
from scene_segmentation import segment_scenes
from fingerprinter import fingerprint

app = FastAPI()


def _save_upload(upload: UploadFile) -> str:
    suffix = os.path.splitext(upload.filename)[1]
    fd, path = tempfile.mkstemp(suffix=suffix)
    with os.fdopen(fd, "wb") as f:
        f.write(upload.file.read())
    return path


@app.post("/tag/audio")
async def tag_audio(file: UploadFile = File(...)):
    path = _save_upload(file)
    tags = {
        "genre": classify_genre(path),
        "mood": detect_mood(path),
        "fingerprint": fingerprint(path),
        "transcript": transcribe_audio(path),
    }
    os.unlink(path)
    return JSONResponse(tags)


@app.post("/tag/video")
async def tag_video(file: UploadFile = File(...)):
    path = _save_upload(file)
    tags = {
        "objects": detect_objects(path),
        "faces": recognize_faces(path),
        "scenes": segment_scenes(path),
        "transcript": transcribe_audio(path),
    }
    os.unlink(path)
    return JSONResponse(tags)


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(app, host="0.0.0.0", port=8000)
