#ifndef MEDIAPLAYER_AUDIOEFFECT_H
#define MEDIAPLAYER_AUDIOEFFECT_H

#include <cstddef>
#include <cstdint>

namespace mediaplayer {

class AudioEffect {
public:
  virtual ~AudioEffect() = default;
  // Process PCM samples in-place. Samples are signed 16-bit interleaved.
  virtual void process(int16_t *samples, size_t sampleCount, int channels, int sampleRate) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOEFFECT_H
