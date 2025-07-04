#ifndef MEDIAPLAYER_VIDEOITEM_H
#define MEDIAPLAYER_VIDEOITEM_H

#include "mediaplayer/VideoOutput.h"
#include <QQuickFramebufferObject>

namespace mediaplayer {

class VideoItemRenderer;

class VideoItem : public QQuickFramebufferObject {
  Q_OBJECT
public:
  VideoItem();
  Renderer *createRenderer() const override;
  void setVideoOutput(std::unique_ptr<VideoOutput> output);

private:
  std::unique_ptr<VideoOutput> m_output;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOITEM_H
