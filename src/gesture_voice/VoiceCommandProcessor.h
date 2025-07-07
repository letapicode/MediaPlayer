#ifndef MEDIAPLAYER_VOICECOMMANDPROCESSOR_H
#define MEDIAPLAYER_VOICECOMMANDPROCESSOR_H

#include <QObject>

namespace mediaplayer {
class MediaPlayerController;

class VoiceCommandProcessor : public QObject {
  Q_OBJECT

public:
  explicit VoiceCommandProcessor(MediaPlayerController *ctrl, QObject *parent = nullptr);

public slots:
  void processText(const QString &text);

signals:
  void commandUnknown(const QString &text);

private:
  MediaPlayerController *m_ctrl;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VOICECOMMANDPROCESSOR_H
