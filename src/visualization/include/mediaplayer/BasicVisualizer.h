#ifndef MEDIAPLAYER_BASICVISUALIZER_H
#define MEDIAPLAYER_BASICVISUALIZER_H

#include "mediaplayer/SimpleFFT.h"
#include "mediaplayer/Visualizer.h"

#include <mutex>
#include <vector>

namespace mediaplayer {

class BasicVisualizer : public Visualizer {
public:
  BasicVisualizer() = default;
  void onAudioPCM(const int16_t *samples, size_t count, int sampleRate, int channels) override;

  /// Thread-safe accessor for the most recent FFT results
  std::vector<float> spectrum() const;

private:
  std::vector<float> m_spectrum;
  mutable std::mutex m_mutex;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_BASICVISUALIZER_H
