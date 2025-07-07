import json
import sys
from vosk import Model, KaldiRecognizer
import wave

COMMANDS_PATH = sys.argv[1]
MODEL_PATH = sys.argv[2]
AUDIO_PATH = sys.argv[3]

with open(COMMANDS_PATH) as f:
    grammar = json.load(f)["phrases"]

model = Model(MODEL_PATH)
rec = KaldiRecognizer(model, 16000, json.dumps(grammar))
wf = wave.open(AUDIO_PATH, "rb")

while True:
    data = wf.readframes(4000)
    if len(data) == 0:
        break
    if rec.AcceptWaveform(data):
        pass

result = json.loads(rec.FinalResult())
text = result.get("text", "")
print("Recognized:", text)

mapping = {
    "play": "play",
    "pause": "pause",
    "next track": "nextTrack",
    "previous track": "previousTrack",
}
for phrase, action in mapping.items():
    if phrase in text:
        print("Action:", action)
        break
else:
    print("Action: none")
