#ifndef MEDIAPLAYER_VISUALIZER_H
#define MEDIAPLAYER_VISUALIZER_H

#include <cstddef>
#include <cstdint>

namespace mediaplayer {

class Visualizer {
public:
  virtual ~Visualizer() = default;
  virtual void onAudioPCM(const int16_t *samples, size_t count, int sampleRate, int channels) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VISUALIZER_H
