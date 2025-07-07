#ifndef MEDIAPLAYER_VOSKRECOGNIZER_H
#define MEDIAPLAYER_VOSKRECOGNIZER_H

#include <QObject>
#include <QString>
#include <atomic>
#include <string>
#include <vector>

struct VoskModel;
struct VoskRecognizer;

namespace mediaplayer {

class VoiceInputQt; // forward declaration

class VoskRecognizer : public QObject {
  Q_OBJECT
public:
  explicit VoskRecognizer(const QString &modelPath, const std::vector<QString> &grammar,
                          QObject *parent = nullptr);
  ~VoskRecognizer() override;

  Q_INVOKABLE void startListening();
  Q_INVOKABLE void stopListening();

  void feedAudio(const int16_t *data, int length);

signals:
  void commandRecognized(const QString &command);

private:
  VoskModel *m_model{nullptr};
  ::VoskRecognizer *m_rec{nullptr};
  VoiceInputQt *m_input{nullptr};
  std::atomic<bool> m_active{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VOSKRECOGNIZER_H
