#ifndef MEDIAPLAYER_AUDIOOUTPUTPULSE_H
#define MEDIAPLAYER_AUDIOOUTPUTPULSE_H

#include "AudioOutput.h"
#include <pulse/error.h>
#include <pulse/simple.h>
#include <string>

namespace mediaplayer {

class AudioOutputPulse : public AudioOutput {
public:
  AudioOutputPulse();
  ~AudioOutputPulse() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;

private:
  pa_simple *m_pa = nullptr;
  pa_sample_spec m_spec{};
  bool m_paused = false;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTPULSE_H
