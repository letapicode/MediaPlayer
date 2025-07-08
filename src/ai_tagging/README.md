# AI Tagging Service

This module provides automatic metadata generation for audio and video files. The
analysis code is written in **Python** and uses libraries such as **PyTorch** and
**ONNX Runtime** for model inference. Python was chosen instead of a C++
implementation to simplify model loading and experimentation with different
frameworks. The C++ core interacts with the Python service over a small HTTP API
using `libcurl`.

Python sources live in `src/ai_tagging/python/` and models are stored under
`src/ai_tagging/python/models/` (not tracked in git). The service exposes two
endpoints:

```
POST /tag/audio  - returns tags for an audio file
POST /tag/video  - returns tags for a video file
```

The C++ layer uploads a media file and receives JSON tags in response.

## Setup

Create a virtual environment and install the dependencies listed in
`requirements.txt`:

```bash
python3 -m venv venv
source venv/bin/activate
pip install -r src/ai_tagging/python/requirements.txt
```

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
```

See `tests/ai_tagging_test.py` for a script that exercises the service and logs
basic performance metrics.

## Performance

Running the test script on a small 30s audio clip typically finishes within a few
seconds on a desktop CPU and uses less than 200 MB of RAM. If processing becomes
slow consider trimming input to the first 30 seconds or reducing video frame
sampling rate in `object_scene_detector.py`.
