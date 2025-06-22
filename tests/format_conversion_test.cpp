#include "mediaplayer/AudioConverter.h"
#include <cassert>
#include <fstream>
#include <vector>

// Simple helper to create a small PCM wav file for tests
static void createTestWav(const std::string &path) {
  const int sampleRate = 44100;
  const int seconds = 1;
  const int16_t amplitude = 1000;
  std::vector<int16_t> samples(sampleRate * seconds);
  for (int i = 0; i < sampleRate * seconds; ++i) {
    samples[i] = (i % 100 < 50 ? amplitude : -amplitude);
  }
  std::ofstream f(path, std::ios::binary);
  int32_t chunkSize = 36 + samples.size() * sizeof(int16_t);
  f.write("RIFF", 4);
  f.write(reinterpret_cast<const char *>(&chunkSize), 4);
  f.write("WAVEfmt ", 8);
  int32_t subChunk1 = 16;
  int16_t audioFormat = 1;
  int16_t numChannels = 1;
  int32_t byteRate = sampleRate * numChannels * sizeof(int16_t);
  int16_t blockAlign = numChannels * sizeof(int16_t);
  int16_t bitsPerSample = 16;
  f.write(reinterpret_cast<const char *>(&subChunk1), 4);
  f.write(reinterpret_cast<const char *>(&audioFormat), 2);
  f.write(reinterpret_cast<const char *>(&numChannels), 2);
  f.write(reinterpret_cast<const char *>(&sampleRate), 4);
  f.write(reinterpret_cast<const char *>(&byteRate), 4);
  f.write(reinterpret_cast<const char *>(&blockAlign), 2);
  f.write(reinterpret_cast<const char *>(&bitsPerSample), 2);
  f.write("data", 4);
  int32_t dataSize = samples.size() * sizeof(int16_t);
  f.write(reinterpret_cast<const char *>(&dataSize), 4);
  f.write(reinterpret_cast<const char *>(samples.data()), dataSize);
}

int main() {
  const std::string wav = "test_input.wav";
  const std::string mp3 = "test_output.mp3";
  createTestWav(wav);
  mediaplayer::AudioConverter conv;
  bool ok = conv.convert(wav, mp3);
  assert(ok && "conversion failed");
  std::ifstream f(mp3, std::ios::binary);
  assert(f.good() && "output file missing");
  return 0;
}
