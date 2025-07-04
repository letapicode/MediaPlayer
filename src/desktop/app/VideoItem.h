#ifndef MEDIAPLAYER_VIDEOITEM_H
#define MEDIAPLAYER_VIDEOITEM_H

#include <QQuickFramebufferObject>

namespace mediaplayer {

class VideoOutputQt;
class VideoItemRenderer;

class VideoItem : public QQuickFramebufferObject {
  Q_OBJECT
  Q_PROPERTY(
      VideoOutputQt *videoOutput READ videoOutput WRITE setVideoOutput NOTIFY videoOutputChanged)

public:
  explicit VideoItem(QQuickItem *parent = nullptr);
  Renderer *createRenderer() const override;

  VideoOutputQt *videoOutput() const { return m_output; }
  void setVideoOutput(VideoOutputQt *output);

signals:
  void videoOutputChanged();

private:
  VideoOutputQt *m_output{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOITEM_H
