#include "mediaplayer/AudioOutputPulse.h"

#include <iostream>

namespace mediaplayer {

AudioOutputPulse::AudioOutputPulse() = default;

AudioOutputPulse::~AudioOutputPulse() { shutdown(); }

bool AudioOutputPulse::init(int sampleRate, int channels) {
  m_spec.format = PA_SAMPLE_S16LE;
  m_spec.rate = sampleRate;
  m_spec.channels = static_cast<uint8_t>(channels);

  int error = 0;
  m_pa = pa_simple_new(nullptr, "MediaPlayer", PA_STREAM_PLAYBACK, nullptr, "playback", &m_spec,
                       nullptr, nullptr, &error);
  if (!m_pa) {
    std::cerr << "PulseAudio init failed: " << pa_strerror(error) << "\n";
    return false;
  }
  return true;
}

void AudioOutputPulse::shutdown() {
  if (m_pa) {
    pa_simple_drain(m_pa, nullptr);
    pa_simple_free(m_pa);
    m_pa = nullptr;
  }
}

int AudioOutputPulse::write(const uint8_t *data, int len) {
  if (!m_pa || m_paused)
    return 0;
  size_t samples = static_cast<size_t>(len) / sizeof(int16_t);
  const int16_t *input = reinterpret_cast<const int16_t *>(data);
  m_buffer.mix(input, samples);

  int16_t temp[1024];
  int error = 0;
  while (m_buffer.available() >= 1024) {
    size_t n = m_buffer.read(temp, 1024);
    if (pa_simple_write(m_pa, temp, n * sizeof(int16_t), &error) < 0) {
      std::cerr << "PulseAudio write failed: " << pa_strerror(error) << "\n";
      return -1;
    }
  }
  return len;
}

void AudioOutputPulse::pause() { m_paused = true; }

void AudioOutputPulse::resume() { m_paused = false; }

void AudioOutputPulse::setVolume(double volume) {
  if (volume < 0.0)
    volume = 0.0;
  else if (volume > 1.0)
    volume = 1.0;
  m_volume = volume;
}

double AudioOutputPulse::volume() const { return m_volume; }

} // namespace mediaplayer
