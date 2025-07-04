#include "AudioOutputQt.h"
#include <QMediaDevices>

using namespace mediaplayer;

AudioOutputQt::AudioOutputQt() : m_device(QMediaDevices::defaultAudioOutput()) {}

AudioOutputQt::AudioOutputQt(const QAudioDevice &device) : m_device(device) {}

AudioOutputQt::~AudioOutputQt() { shutdown(); }

bool AudioOutputQt::init(int sampleRate, int channels) {
  shutdown();
  QAudioFormat fmt;
  fmt.setSampleRate(sampleRate);
  fmt.setChannelCount(channels);
  fmt.setSampleFormat(QAudioFormat::Int16);
  if (m_device.isNull())
    m_device = QMediaDevices::defaultAudioOutput();
  m_sink = new QAudioSink(m_device, fmt);
  m_io = m_sink->start();
  return m_io != nullptr;
}

void AudioOutputQt::shutdown() {
  if (m_sink) {
    m_sink->stop();
    delete m_sink;
    m_sink = nullptr;
    m_io = nullptr;
  }
}

int AudioOutputQt::write(const uint8_t *data, int len) {
  if (!m_io)
    return 0;
  return static_cast<int>(m_io->write(reinterpret_cast<const char *>(data), len));
}

void AudioOutputQt::pause() {
  if (m_sink)
    m_sink->suspend();
}

void AudioOutputQt::resume() {
  if (m_sink)
    m_sink->resume();
}

void AudioOutputQt::setVolume(double volume) {
  if (m_sink)
    m_sink->setVolume(volume);
}

double AudioOutputQt::volume() const { return m_sink ? m_sink->volume() : 1.0; }

void AudioOutputQt::setDevice(const QAudioDevice &device) { m_device = device; }
