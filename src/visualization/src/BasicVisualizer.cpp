#include "mediaplayer/BasicVisualizer.h"

namespace mediaplayer {

void BasicVisualizer::onAudioPCM(const int16_t *samples, size_t count, int /*sampleRate*/,
                                 int channels) {
  if (!samples || count == 0)
    return;

  size_t frameCount = count / (channels > 0 ? channels : 1);
  std::vector<int16_t> mono(frameCount);
  for (size_t i = 0; i < frameCount; ++i) {
    int32_t sum = 0;
    for (int ch = 0; ch < channels; ++ch)
      sum += samples[i * channels + ch];
    mono[i] = static_cast<int16_t>(sum / (channels > 0 ? channels : 1));
  }

  auto spectrum = simpleFFT(mono.data(), mono.size());
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_spectrum = std::move(spectrum);
  }
}

const std::vector<float> &BasicVisualizer::spectrum() const { return m_spectrum; }

} // namespace mediaplayer
