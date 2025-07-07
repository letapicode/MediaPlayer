# Gesture & Voice Control

This module integrates the [Vosk](https://alphacephei.com/vosk/) offline speech recognition library. The `VoskRecognizer` implementation resides in this directory and expects an English model installed locally. To try it out:

1. Download the small English model from the [Vosk models page](https://alphacephei.com/vosk/models) and extract it somewhere, e.g. `models/vosk-model-small-en-us-0.15`.
2. Pass the absolute model path when constructing `VoskRecognizer` in your
   application.
3. Ensure your microphone is accessible. On desktop the recognizer uses Qt
   Multimedia to capture audio.

Only the model files themselves are required at runtime; they are not stored in
the repository due to their size.

## Command grammar

`commands.json` lists the phrases that the recognizer will listen for. The file
is loaded by `VoskRecognizer` and passed to Vosk as a grammar to improve
accuracy. To extend the grammar, add new phrases to the `phrases` array. Example:

```json
{
  "phrases": [
    "play",
    "pause",
    "next track",
    "previous track"
  ]
}
```

After editing the JSON file rebuild the application so the recognizer reloads
the grammar.
