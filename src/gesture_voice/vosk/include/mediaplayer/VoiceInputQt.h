#ifndef MEDIAPLAYER_VOICEINPUTQT_H
#define MEDIAPLAYER_VOICEINPUTQT_H

#include <QAudioDevice>
#include <QAudioSource>
#include <QObject>

namespace mediaplayer {

class VoskRecognizer;

class VoiceInputQt : public QObject {
  Q_OBJECT
public:
  explicit VoiceInputQt(VoskRecognizer *recognizer, QObject *parent = nullptr);
  ~VoiceInputQt() override;

  void start();
  void stop();

private slots:
  void readMore();

private:
  QAudioDevice m_device;
  QAudioSource *m_source{nullptr};
  QIODevice *m_io{nullptr};
  VoskRecognizer *m_recognizer{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VOICEINPUTQT_H
