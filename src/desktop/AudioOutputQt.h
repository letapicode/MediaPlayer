#ifndef MEDIAPLAYER_AUDIOOUTPUTQT_H
#define MEDIAPLAYER_AUDIOOUTPUTQT_H

#include "mediaplayer/AudioOutput.h"
#include <QAudioDevice>
#include <QAudioSink>

namespace mediaplayer {

class AudioOutputQt : public AudioOutput {
public:
  AudioOutputQt();
  explicit AudioOutputQt(const QAudioDevice &device);
  ~AudioOutputQt() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;
  void setVolume(double volume) override;
  double volume() const override;

  void setDevice(const QAudioDevice &device);

private:
  QAudioDevice m_device;
  QAudioSink *m_sink{nullptr};
  QIODevice *m_io{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTQT_H
