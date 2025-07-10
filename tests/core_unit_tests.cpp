#include "mediaplayer/AudioConverter.h"
#include "mediaplayer/MediaPlayer.h"
#include "mediaplayer/NullAudioOutput.h"
#include "mediaplayer/NullVideoOutput.h"
#include <fstream>
#include <gtest/gtest.h>
#include <thread>

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

class MediaPlayerTest : public ::testing::Test {
protected:
  void SetUp() override {
    wavPath = "test_input.wav";
    mp3Path = "test_output.mp3";
    createTestWav(wavPath);
    player.setAudioOutput(std::make_unique<NullAudioOutput>());
    player.setVideoOutput(std::make_unique<NullVideoOutput>());
    ASSERT_TRUE(player.open(wavPath));
  }

  void TearDown() override {
    player.stop();
    std::remove(wavPath.c_str());
    std::remove(mp3Path.c_str());
  }

  MediaPlayer player;
  std::string wavPath;
  std::string mp3Path;
};

TEST_F(MediaPlayerTest, OpenFileReadsDuration) {
  EXPECT_NEAR(player.metadata().duration, 1.0, 0.1);
}

TEST_F(MediaPlayerTest, PauseStopsPositionAdvance) {
  player.play();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  player.pause();
  double pos = player.position();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  EXPECT_NEAR(player.position(), pos, 0.05);
}

TEST_F(MediaPlayerTest, FormatConversionWorks) {
  AudioConverter conv;
  AudioEncodeOptions opts;
  opts.bitrate = 64000;
  bool ok = conv.convert(wavPath, mp3Path, opts);
  EXPECT_TRUE(ok);
  std::ifstream f(mp3Path, std::ios::binary);
  EXPECT_TRUE(f.good());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
