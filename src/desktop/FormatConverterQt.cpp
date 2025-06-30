#include "FormatConverterQt.h"

using namespace mediaplayer;

FormatConverterQt::FormatConverterQt(QObject *parent) : QObject(parent) {}

void FormatConverterQt::convertAudio(const QString &input, const QString &output,
                                     const AudioEncodeOptions &options) {
  m_converter.convertAudioAsync(
      input.toStdString(), output.toStdString(), options,
      [this](float p) { emit progressChanged(p); },
      [this](bool ok) { emit conversionFinished(ok && !m_converter.isCancelled()); });
}

void FormatConverterQt::convertVideo(const QString &input, const QString &output,
                                     const VideoEncodeOptions &options) {
  m_converter.convertVideoAsync(
      input.toStdString(), output.toStdString(), options,
      [this](float p) { emit progressChanged(p); },
      [this](bool ok) { emit conversionFinished(ok && !m_converter.isCancelled()); });
}

void FormatConverterQt::cancel() { m_converter.cancel(); }
