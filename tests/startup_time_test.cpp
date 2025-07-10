#include "mediaplayer/MediaPlayer.h"
#include "mediaplayer/NullAudioOutput.h"
#include "mediaplayer/NullVideoOutput.h"
#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

using mediaplayer::MediaPlayer;
using mediaplayer::NullAudioOutput;
using mediaplayer::NullVideoOutput;

static void createTestWav(const std::string &path) {
  const int sampleRate = 8000;
  const int seconds = 1;
  std::vector<int16_t> samples(sampleRate * seconds, 0);
  std::ofstream f(path, std::ios::binary);
  int32_t chunkSize = 36 + samples.size() * sizeof(int16_t);
  f.write("RIFF", 4);
  f.write(reinterpret_cast<const char *>(&chunkSize), 4);
  f.write("WAVEfmt ", 8);
  int32_t subChunk1 = 16;
  f.write(reinterpret_cast<const char *>(&subChunk1), 4);
  int16_t audioFormat = 1;
  f.write(reinterpret_cast<const char *>(&audioFormat), 2);
  int16_t channels = 1;
  f.write(reinterpret_cast<const char *>(&channels), 2);
  int32_t rate = sampleRate;
  f.write(reinterpret_cast<const char *>(&rate), 4);
  int32_t byteRate = rate * channels * 2;
  f.write(reinterpret_cast<const char *>(&byteRate), 4);
  int16_t blockAlign = channels * 2;
  f.write(reinterpret_cast<const char *>(&blockAlign), 2);
  int16_t bits = 16;
  f.write(reinterpret_cast<const char *>(&bits), 2);
  f.write("data", 4);
  int32_t dataSize = samples.size() * sizeof(int16_t);
  f.write(reinterpret_cast<const char *>(&dataSize), 4);
  f.write(reinterpret_cast<const char *>(samples.data()), dataSize);
}

TEST(Performance, StartupTimeUnder2s) {
  const std::string file = "startup_test.wav";
  createTestWav(file);
  MediaPlayer player;
  player.setAudioOutput(std::make_unique<NullAudioOutput>());
  player.setVideoOutput(std::make_unique<NullVideoOutput>());
  auto start = std::chrono::steady_clock::now();
  ASSERT_TRUE(player.open(file));
  auto end = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  EXPECT_LT(ms, 2000);
  std::remove(file.c_str());
}
