#include "mediaplayer/BasicVisualizer.h"
#include <cassert>
#include <cmath>
#include <vector>

int main() {
  const int sampleRate = 44100;
  const int channels = 1;
  const int seconds = 1;
  const double freq = 440.0;
  const int16_t amplitude = 16000;
  std::vector<int16_t> pcm(sampleRate * seconds * channels);
  for (int i = 0; i < static_cast<int>(pcm.size()); ++i) {
    double t = static_cast<double>(i) / sampleRate;
    pcm[i] = static_cast<int16_t>(std::sin(2.0 * M_PI * freq * t) * amplitude);
  }

  mediaplayer::BasicVisualizer vis;
  vis.onAudioPCM(pcm.data(), pcm.size(), sampleRate, channels);
  auto spec = vis.spectrum();
  bool nonZero = false;
  for (float v : spec) {
    if (v > 0.0f) {
      nonZero = true;
      break;
    }
  }
  assert(nonZero && "Visualizer output is all zeros");
  return 0;
}
