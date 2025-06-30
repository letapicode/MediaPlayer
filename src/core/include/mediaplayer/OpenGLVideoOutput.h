#ifndef MEDIAPLAYER_OPENGLVIDEOOUTPUT_H
#define MEDIAPLAYER_OPENGLVIDEOOUTPUT_H

#include "VideoFrame.h"
#include "VideoOutput.h"
#include <GLFW/glfw3.h>

extern "C" {
#include <libswscale/swscale.h>
}

namespace mediaplayer {

class OpenGLVideoOutput : public VideoOutput {
public:
  OpenGLVideoOutput();
  ~OpenGLVideoOutput() override;

  bool init(int width, int height) override;
  void shutdown() override;
  void displayFrame(const uint8_t *rgba, int linesize) override;
  void displayFrame(const VideoFrame &frame) override;

private:
  GLFWwindow *m_window{nullptr};
  unsigned int m_texture{0};
  int m_width{0};
  int m_height{0};
  SwsContext *m_swsCtx{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_OPENGLVIDEOOUTPUT_H
