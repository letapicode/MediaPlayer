#ifndef MEDIAPLAYER_MICROPHONEINPUT_H
#define MEDIAPLAYER_MICROPHONEINPUT_H

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSource>
#include <QObject>

namespace mediaplayer {

class MicrophoneInput : public QObject {
  Q_OBJECT

public:
  explicit MicrophoneInput(QObject *parent = nullptr);
  Q_INVOKABLE void start();
  Q_INVOKABLE void stop();

signals:
  void audioDataReady(const QByteArray &data);

private:
  QAudioFormat m_format;
  QAudioDevice m_device;
  QAudioSource *m_source{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MICROPHONEINPUT_H
