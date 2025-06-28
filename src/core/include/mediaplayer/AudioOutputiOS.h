#ifndef MEDIAPLAYER_AUDIOOUTPUTIOS_H
#define MEDIAPLAYER_AUDIOOUTPUTIOS_H

#include "AudioOutput.h"
#include "RingBuffer.h"

namespace mediaplayer {

class AudioOutputiOS : public AudioOutput {
public:
  AudioOutputiOS();
  ~AudioOutputiOS() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;
  void setVolume(double volume) override;
  double volume() const override;

private:
  void *m_engine{nullptr};
  void *m_player{nullptr};
  void *m_format{nullptr};
  bool m_paused{false};
  double m_volume{1.0};
  void scheduleNextBuffer();
  RingBuffer m_buffer{16384};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTIOS_H
