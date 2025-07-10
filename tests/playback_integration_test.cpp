#include "mediaplayer/MediaPlayer.h"
#include "mediaplayer/NullAudioOutput.h"
#include "mediaplayer/NullVideoOutput.h"
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

using namespace mediaplayer;

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
  const std::string path = "tests/sample_media/sample.wav";
  if (!std::filesystem::exists(path))
    createTestWav(path);

  MediaPlayer player;
  player.setAudioOutput(std::make_unique<NullAudioOutput>());
  player.setVideoOutput(std::make_unique<NullVideoOutput>());

  enum class State { Stopped, Playing, Ended } state = State::Stopped;
  PlaybackCallbacks cb;
  cb.onPlay = [&]() { state = State::Playing; };
  cb.onFinished = [&]() { state = State::Ended; };
  player.setCallbacks(cb);

  assert(player.open(path));
  player.play();

  auto start = std::chrono::steady_clock::now();
  while (state != State::Ended &&
         std::chrono::steady_clock::now() - start < std::chrono::seconds(5)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  player.stop();
  assert(state == State::Ended);
  std::cout << "Playback integration test passed\n";
  return 0;
}
