#ifndef MEDIAPLAYER_VIDEOOUTPUTQT_H
#define MEDIAPLAYER_VIDEOOUTPUTQT_H

#include "mediaplayer/VideoOutput.h"
#include <QImage>
#include <QObject>
#include <mutex>

struct SwsContext;

namespace mediaplayer {

class VideoOutputQt : public QObject, public VideoOutput {
  Q_OBJECT
public:
  explicit VideoOutputQt(QObject *parent = nullptr);
  ~VideoOutputQt() override;

  bool init(int width, int height) override;
  void shutdown() override;
  void displayFrame(const uint8_t *rgba, int linesize) override;
  void displayFrame(const VideoFrame &frame) override;

  QImage currentFrame() const;

signals:
  void frameReady();

private:
  mutable std::mutex m_mutex;
  QImage m_image;
  SwsContext *m_swsCtx{nullptr};
  int m_width{0};
  int m_height{0};
};

void registerVideoOutputQtQmlType();

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOOUTPUTQT_H
