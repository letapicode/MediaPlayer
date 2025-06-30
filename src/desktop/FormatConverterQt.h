#ifndef MEDIAPLAYER_FORMATCONVERTERQT_H
#define MEDIAPLAYER_FORMATCONVERTERQT_H

#include "mediaplayer/FormatConverter.h"
#include <QObject>

namespace mediaplayer {

class FormatConverterQt : public QObject {
  Q_OBJECT
public:
  explicit FormatConverterQt(QObject *parent = nullptr);

  Q_INVOKABLE void convertAudio(const QString &input, const QString &output,
                                const AudioEncodeOptions &options = {});
  Q_INVOKABLE void convertVideo(const QString &input, const QString &output,
                                const VideoEncodeOptions &options = {});
  Q_INVOKABLE void cancel();

signals:
  void progressChanged(float progress);
  void conversionFinished(bool success);

private:
  FormatConverter m_converter;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_FORMATCONVERTERQT_H
