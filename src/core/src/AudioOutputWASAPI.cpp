#ifdef _WIN32
#include "mediaplayer/AudioOutputWASAPI.h"
#include <algorithm>
#include <cstring>
#include <iostream>

namespace mediaplayer {

AudioOutputWASAPI::AudioOutputWASAPI() : m_comInit(false) {}

AudioOutputWASAPI::~AudioOutputWASAPI() { shutdown(); }

bool AudioOutputWASAPI::init(int sampleRate, int channels) {
  HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
    std::cerr << "CoInitializeEx failed: 0x" << std::hex << hr << '\n';
    return false;
  }
  m_comInit = SUCCEEDED(hr);

  ComPtr<IMMDeviceEnumerator> enumerator;
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                        IID_PPV_ARGS(&enumerator));
  if (FAILED(hr)) {
    std::cerr << "CoCreateInstance failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
  if (FAILED(hr)) {
    std::cerr << "GetDefaultAudioEndpoint failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  hr = m_device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, IID_PPV_ARGS(&m_client));
  if (FAILED(hr)) {
    std::cerr << "Activate IAudioClient failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  WAVEFORMATEX waveFmt{};
  waveFmt.wFormatTag = WAVE_FORMAT_PCM;
  waveFmt.nChannels = static_cast<WORD>(channels);
  waveFmt.nSamplesPerSec = sampleRate;
  waveFmt.wBitsPerSample = 16;
  waveFmt.nBlockAlign = waveFmt.nChannels * waveFmt.wBitsPerSample / 8;
  waveFmt.nAvgBytesPerSec = waveFmt.nSamplesPerSec * waveFmt.nBlockAlign;
  waveFmt.cbSize = 0;

  REFERENCE_TIME bufferDuration = 1000000;
  hr = m_client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, bufferDuration, 0, &waveFmt, nullptr);
  if (FAILED(hr)) {
    std::cerr << "IAudioClient::Initialize failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  hr = m_client->GetBufferSize(&m_bufferFrames);
  if (FAILED(hr)) {
    std::cerr << "GetBufferSize failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  hr = m_client->GetService(__uuidof(IAudioRenderClient), IID_PPV_ARGS(&m_render));
  if (FAILED(hr)) {
    std::cerr << "GetService IAudioRenderClient failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }

  m_format.reset(reinterpret_cast<WAVEFORMATEX *>(CoTaskMemAlloc(sizeof(WAVEFORMATEX))));
  if (!m_format) {
    std::cerr << "CoTaskMemAlloc failed for format\n";
    shutdown();
    return false;
  }
  *m_format = waveFmt;

  hr = m_client->Start();
  if (FAILED(hr)) {
    std::cerr << "IAudioClient::Start failed: 0x" << std::hex << hr << '\n';
    shutdown();
    return false;
  }
  m_paused = false;
  return true;
}

void AudioOutputWASAPI::shutdown() {
  if (m_client) {
    m_client->Stop();
  }
  m_render.reset();
  m_client.reset();
  m_device.reset();
  m_format.reset();
  if (m_comInit) {
    CoUninitialize();
    m_comInit = false;
  }
}

int AudioOutputWASAPI::write(const uint8_t *data, int len) {
  if (!m_render || m_paused)
    return 0;

  size_t samples = static_cast<size_t>(len) / sizeof(int16_t);
  const int16_t *input = reinterpret_cast<const int16_t *>(data);
  m_buffer.mix(input, samples);

  while (true) {
    UINT32 padding = 0;
    if (FAILED(m_client->GetCurrentPadding(&padding)))
      break;

    UINT32 framesAvailable = m_bufferFrames - padding;
    if (framesAvailable == 0 || m_buffer.available() < framesAvailable * m_format->nChannels)
      break;

    BYTE *buffer = nullptr;
    if (FAILED(m_render->GetBuffer(framesAvailable, &buffer)))
      break;

    size_t toRead = framesAvailable * m_format->nChannels;
    std::vector<int16_t> temp(toRead);
    m_buffer.read(temp.data(), toRead);
    std::memcpy(buffer, temp.data(), toRead * sizeof(int16_t));
    m_render->ReleaseBuffer(framesAvailable, 0);
  }
  return len;
}

void AudioOutputWASAPI::pause() {
  if (m_client && !m_paused) {
    m_client->Stop();
    m_paused = true;
  }
}

void AudioOutputWASAPI::resume() {
  if (m_client && m_paused) {
    m_client->Start();
    m_paused = false;
  }
}

void AudioOutputWASAPI::setVolume(double volume) {
  if (volume < 0.0)
    volume = 0.0;
  else if (volume > 1.0)
    volume = 1.0;
  m_volume = volume;
}

double AudioOutputWASAPI::volume() const { return m_volume; }

} // namespace mediaplayer
#endif
