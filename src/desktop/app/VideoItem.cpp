#include "VideoItem.h"

using namespace mediaplayer;

class VideoItemRenderer : public QQuickFramebufferObject::Renderer {
public:
  void render() override {}
};

VideoItem::VideoItem() {}

QQuickFramebufferObject::Renderer *VideoItem::createRenderer() const {
  return new VideoItemRenderer();
}

void VideoItem::setVideoOutput(std::unique_ptr<VideoOutput> output) {
  m_output = std::move(output);
}
