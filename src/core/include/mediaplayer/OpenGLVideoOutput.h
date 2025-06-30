#ifndef MEDIAPLAYER_OPENGLVIDEOOUTPUT_H
#define MEDIAPLAYER_OPENGLVIDEOOUTPUT_H

#include "VideoFrame.h"
#include "VideoOutput.h"
#include <GLFW/glfw3.h>

namespace mediaplayer {

class OpenGLVideoOutput : public VideoOutput {
public:
  OpenGLVideoOutput();
  ~OpenGLVideoOutput() override;

  bool init(int width, int height) override { return init(width, height, nullptr); }
  bool init(int width, int height, void *externalContext);
  void shutdown() override;
  void displayFrame(const VideoFrame &frame) override;

private:
  GLFWwindow *m_window{nullptr};
  bool m_external{false};
  unsigned int m_texY{0};
  unsigned int m_texU{0};
  unsigned int m_texV{0};
  unsigned int m_program{0};
  int m_width{0};
  int m_height{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_OPENGLVIDEOOUTPUT_H
