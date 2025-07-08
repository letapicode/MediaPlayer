# AI Tagging Service

This module provides automatic metadata generation for audio and video files. The
analysis code is written in **Python** and uses libraries such as **PyTorch** and
**ONNX Runtime** for model inference. Python was chosen instead of a C++
implementation to simplify model loading and experimentation with different
frameworks. Python gives us maximum flexibility to swap out models or
experiment with new libraries quickly. The C++ core interacts with the Python
service over a small HTTP API using `libcurl`.

Python sources live in `src/ai_tagging/python/` and models are stored under
`src/ai_tagging/python/models/` (not tracked in git). The service exposes
asynchronous endpoints:

```
POST /tag/audio  - queue tagging of an audio file, returns `{"job_id": N}`
POST /tag/video  - queue tagging of a video file, returns `{"job_id": N}`
GET  /result/{id} - retrieve the JSON tags for a completed job
```

The C++ layer uploads a media file, then polls `/result/{job_id}` to obtain the
JSON tags once processing finishes.

## Setup

Create a virtual environment and install the dependencies listed in
`requirements.txt`:


```bash
python3 -m venv venv
source venv/bin/activate
pip install -r src/ai_tagging/python/requirements.txt
```

The models used by the individual modules are not checked in. Create the
`src/ai_tagging/python/models/` directory (if it does not exist) and place
downloaded model files there. Each Python module lists the specific model name
expected.

Download the required models into `src/ai_tagging/python/models/`. Instructions
for each submodule are noted in the corresponding Python files.

## Example Usage

```python
from genre_classifier import classify_genre
from mood_detector import detect_mood

print(classify_genre("song.wav"))
print(detect_mood("song.wav"))
```

The API server can be started with:

```bash
python src/ai_tagging/python/service_api.py
# or
uvicorn service_api:app
```

See `tests/ai_tagging_test.py` for a script that exercises the service and logs
basic performance metrics.

## Performance

Running the test script on a small 30s audio clip typically finishes within a few
seconds on a desktop CPU and uses less than 200 MB of RAM. If processing becomes
slow consider trimming input to the first 30 seconds or reducing video frame
sampling rate in `object_scene_detector.py`.
The asynchronous worker now uses a pool of background threads so several jobs can
run in parallel, greatly improving throughput while still keeping requests
non-blocking.
