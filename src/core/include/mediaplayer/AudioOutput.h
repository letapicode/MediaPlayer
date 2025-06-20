#ifndef MEDIAPLAYER_AUDIOOUTPUT_H
#define MEDIAPLAYER_AUDIOOUTPUT_H

#include <cstdint>

namespace mediaplayer {

class AudioOutput {
public:
  virtual ~AudioOutput() = default;
  virtual bool init(int sampleRate, int channels) = 0;
  virtual void shutdown() = 0;
  virtual int write(const uint8_t *data, int len) = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUT_H
