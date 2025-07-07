#include "mediaplayer/VoiceInputQt.h"
#include "mediaplayer/VoskRecognizer.h"
#include <QIODevice>
#include <QMediaDevices>

using namespace mediaplayer;

VoiceInputQt::VoiceInputQt(VoskRecognizer *recognizer, QObject *parent)
    : QObject(parent), m_device(QMediaDevices::defaultAudioInput()), m_recognizer(recognizer) {}

VoiceInputQt::~VoiceInputQt() { stop(); }

void VoiceInputQt::start() {
  if (m_source)
    return;
  QAudioFormat fmt;
  fmt.setSampleRate(16000);
  fmt.setChannelCount(1);
  fmt.setSampleFormat(QAudioFormat::Int16);
  if (m_device.isNull())
    m_device = QMediaDevices::defaultAudioInput();
  m_source = new QAudioSource(m_device, fmt, this);
  m_io = m_source->start();
  connect(m_io, &QIODevice::readyRead, this, &VoiceInputQt::readMore);
}

void VoiceInputQt::stop() {
  if (!m_source)
    return;
  disconnect(m_io, nullptr, this, nullptr);
  m_source->stop();
  delete m_source;
  m_source = nullptr;
  m_io = nullptr;
}

void VoiceInputQt::readMore() {
  if (!m_source || !m_recognizer)
    return;
  QByteArray data = m_io->readAll();
  m_recognizer->feedAudio(reinterpret_cast<const int16_t *>(data.constData()),
                          data.size() / sizeof(int16_t));
}
