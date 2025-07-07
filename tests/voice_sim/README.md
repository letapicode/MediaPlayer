# Voice Simulation Test

This script uses the `vosk` Python package to recognize voice commands from
pre-recorded WAV files. Usage:

```bash
python3 run.py ../../src/gesture_voice/commands.json /path/to/vosk-model path/to/audio.wav
```

The script prints the recognized text and the player action that would be
triggered. Provide your own Vosk model and sample recordings.
