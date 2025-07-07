# Gesture & Voice Control

This module contains helpers used by all platforms to recognize and handle
simple voice commands. Vosk is used for offline speech recognition.

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
