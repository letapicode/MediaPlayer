#ifndef MEDIAPLAYER_AUDIOOUTPUTWASAPI_H
#define MEDIAPLAYER_AUDIOOUTPUTWASAPI_H

#include "AudioOutput.h"
#include "ComPtr.h"
#include "RingBuffer.h"
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
  void setVolume(double volume) override;
  double volume() const override;

private:
  ComPtr<IMMDevice> m_device;
  ComPtr<IAudioClient> m_client;
  ComPtr<IAudioRenderClient> m_render;
  std::unique_ptr<WAVEFORMATEX, void (*)(void *)> m_format{nullptr, CoTaskMemFree};
  UINT32 m_bufferFrames{0};
  bool m_paused{false};
  double m_volume{1.0};
  RingBuffer m_buffer{16384};
};
#endif

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTWASAPI_H
