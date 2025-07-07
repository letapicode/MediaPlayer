#ifndef MEDIAPLAYER_VOSKRECOGNIZER_H
#define MEDIAPLAYER_VOSKRECOGNIZER_H

#include <QObject>
#include <memory>

namespace mediaplayer {

class VoskRecognizer : public QObject {
  Q_OBJECT

public:
  explicit VoskRecognizer(QObject *parent = nullptr);
  Q_INVOKABLE bool loadModel(const QString &modelPath);
  Q_INVOKABLE bool start();
  Q_INVOKABLE void stop();
  Q_INVOKABLE void feedAudio(const QByteArray &data);
  Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
  bool isRunning() const;

signals:
  void textRecognized(const QString &text);
  void error(const QString &message);
  void runningChanged(bool running);

private:
  void loadGrammar();
  struct Impl;
  std::unique_ptr<Impl> m;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VOSKRECOGNIZER_H
