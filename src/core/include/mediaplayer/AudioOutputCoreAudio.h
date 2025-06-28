#ifndef MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H
#define MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H

#include "AudioOutput.h"
#include <AudioToolbox/AudioToolbox.h>

namespace mediaplayer {

class AudioOutputCoreAudio : public AudioOutput {
public:
  AudioOutputCoreAudio();
  ~AudioOutputCoreAudio() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;
  void setVolume(double volume) override;
  double volume() const override;

private:
  AudioQueueRef m_queue{nullptr};
  AudioStreamBasicDescription m_format{};
  bool m_started{false};
  bool m_paused{false};
  double m_volume{1.0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H
