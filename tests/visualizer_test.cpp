#include "mediaplayer/Visualizer.h"
#include <gtest/gtest.h>

using mediaplayer::Visualizer;

class DummyVis : public Visualizer {
public:
  void onAudioPCM(const int16_t *samples, size_t count, int, int) override {
    called = count > 0 && samples;
  }
  bool called{false};
};

TEST(Visualizer, CallbackInvoked) {
  DummyVis vis;
  int16_t sample[2] = {0, 0};
  vis.onAudioPCM(sample, 2, 44100, 2);
  EXPECT_TRUE(vis.called);
}
