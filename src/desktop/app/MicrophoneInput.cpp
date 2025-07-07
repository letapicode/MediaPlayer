#include "MicrophoneInput.h"
#include <QIODevice>
#include <QMediaDevices>
#include <QObject>

using namespace mediaplayer;

MicrophoneInput::MicrophoneInput(QObject *parent) : QObject(parent) {
  m_device = QMediaDevices::defaultAudioInput();
  m_format.setSampleRate(16000);
  m_format.setChannelCount(1);
  m_format.setSampleFormat(QAudioFormat::Int16);
}

void MicrophoneInput::start() {
  stop();
  m_source = new QAudioSource(m_device, m_format, this);
  QIODevice *io = m_source->start();
  if (!io) {
    emit errorOccurred(tr("Failed to access microphone"));
    return;
  }
  connect(m_source, &QAudioSource::errorChanged, this, [this](QAudio::Error e) {
    if (e != QAudio::NoError)
      emit errorOccurred(tr("Microphone error"));
  });
  connect(io, &QIODevice::readyRead, this, [this, io]() {
    QByteArray data = io->readAll();
    if (!data.isEmpty())
      emit audioDataReady(data);
  });
}

void MicrophoneInput::stop() {
  if (m_source) {
    m_source->stop();
    m_source->deleteLater();
    m_source = nullptr;
  }
}
