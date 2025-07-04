#include "VideoItem.h"
#include "../VideoOutputQt.h"
#include <QOpenGLFramebufferObject>
#include <QPainter>

using namespace mediaplayer;

class VideoItemRenderer : public QQuickFramebufferObject::Renderer {
public:
  explicit VideoItemRenderer(VideoItem *item) : m_item(item) {}

  void synchronize(QQuickFramebufferObject *item) override {
    m_item = static_cast<VideoItem *>(item);
    m_output = m_item->videoOutput();
  }

  QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::CombinedDepthStencil);
  }

  void render() override {
    if (!m_output)
      return;
    QImage img = m_output->currentFrame();
    if (img.isNull())
      return;
    QOpenGLFramebufferObject *fbo = framebufferObject();
    QPainter p(fbo);
    p.drawImage(QRect(QPoint(0, 0), fbo->size()), img);
    p.end();
  }

private:
  VideoItem *m_item{nullptr};
  VideoOutputQt *m_output{nullptr};
};

VideoItem::VideoItem(QQuickItem *parent) : QQuickFramebufferObject(parent) {}

QQuickFramebufferObject::Renderer *VideoItem::createRenderer() const {
  return new VideoItemRenderer(const_cast<VideoItem *>(this));
}

void VideoItem::setVideoOutput(VideoOutputQt *output) {
  if (m_output == output)
    return;
  if (m_output)
    disconnect(m_output, &VideoOutputQt::frameReady, this, &VideoItem::update);
  m_output = output;
  if (m_output)
    connect(m_output, &VideoOutputQt::frameReady, this, &VideoItem::update, Qt::DirectConnection);
  emit videoOutputChanged();
}
