#ifndef MEDIAPLAYER_METALVIDEOOUTPUT_H
#define MEDIAPLAYER_METALVIDEOOUTPUT_H

#include "VideoFrame.h"
#include "VideoOutput.h"
#include <cstdint>

namespace mediaplayer {

class MetalVideoOutput : public VideoOutput {
public:
  MetalVideoOutput();
  ~MetalVideoOutput() override;

  bool init(int width, int height) override;
  void shutdown() override;
  void displayFrame(const uint8_t *rgba, int linesize) override;
  void displayFrame(const VideoFrame &frame);

private:
  void *m_layer{nullptr};
  void *m_device{nullptr};
  void *m_commandQueue{nullptr};
  void *m_pipelineState{nullptr};
  void *m_textureY{nullptr};
  void *m_textureU{nullptr};
  void *m_textureV{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_METALVIDEOOUTPUT_H
