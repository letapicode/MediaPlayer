#ifndef MEDIAPLAYER_AUDIOOUTPUTWASAPI_H
#define MEDIAPLAYER_AUDIOOUTPUTWASAPI_H

#include "AudioOutput.h"
#ifdef _WIN32
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#endif

namespace mediaplayer {

#ifdef _WIN32
class AudioOutputWASAPI : public AudioOutput {
public:
  AudioOutputWASAPI();
  ~AudioOutputWASAPI() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;

private:
  IMMDevice *m_device{nullptr};
  IAudioClient *m_client{nullptr};
  IAudioRenderClient *m_render{nullptr};
  WAVEFORMATEX *m_format{nullptr};
  UINT32 m_bufferFrames{0};
  bool m_paused{false};
};
#endif

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTWASAPI_H
