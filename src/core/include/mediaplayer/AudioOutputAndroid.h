#ifndef MEDIAPLAYER_AUDIOOUTPUTANDROID_H
#define MEDIAPLAYER_AUDIOOUTPUTANDROID_H

#include "AudioOutput.h"
#include "RingBuffer.h"
#ifdef __ANDROID__
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <aaudio/AAudio.h>
#include <cstdint>

namespace mediaplayer {

class AudioOutputAndroid : public AudioOutput {
public:
  AudioOutputAndroid();
  ~AudioOutputAndroid() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;
  void setVolume(double volume) override;
  double volume() const override;

private:
  bool initAAudio(int sampleRate, int channels);
  bool initOpenSL(int sampleRate, int channels);

  bool m_useAAudio{false};
  double m_volume{1.0};

  // AAudio
  AAudioStream *m_stream{nullptr};

  // OpenSL ES
  SLObjectItf m_engineObj{nullptr};
  SLEngineItf m_engine{nullptr};
  SLObjectItf m_outputMixObj{nullptr};
  SLObjectItf m_playerObj{nullptr};
  SLPlayItf m_player{nullptr};
  SLAndroidSimpleBufferQueueItf m_bufferQueue{nullptr};
  SLVolumeItf m_volumeItf{nullptr};
  RingBuffer m_buffer{16384};
};

} // namespace mediaplayer

#endif // __ANDROID__
#endif // MEDIAPLAYER_AUDIOOUTPUTANDROID_H
