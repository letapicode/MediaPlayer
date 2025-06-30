#ifndef MEDIAPLAYER_AUDIOEFFECT_H
#define MEDIAPLAYER_AUDIOEFFECT_H

#include <cstddef>
#include <cstdint>

namespace mediaplayer {

class AudioEffect {
public:
  virtual ~AudioEffect() = default;
  virtual void process(int16_t *samples, size_t count) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOEFFECT_H
