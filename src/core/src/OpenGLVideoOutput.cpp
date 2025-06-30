#include "mediaplayer/OpenGLVideoOutput.h"
#include "mediaplayer/VideoFrame.h"
#include <libswscale/swscale.h>
extern "C" {
#include <libavutil/avutil.h>
}

#include <GL/gl.h>
#include <iostream>
#include <vector>

namespace mediaplayer {

OpenGLVideoOutput::OpenGLVideoOutput() = default;

OpenGLVideoOutput::~OpenGLVideoOutput() { shutdown(); }

bool OpenGLVideoOutput::init(int width, int height) {
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW\n";
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  m_window = glfwCreateWindow(width, height, "MediaPlayer", nullptr, nullptr);
  if (!m_window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(m_window);
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_width = width;
  m_height = height;
  return true;
}

void OpenGLVideoOutput::shutdown() {
  if (m_texture) {
    glDeleteTextures(1, &m_texture);
    m_texture = 0;
  }
  if (m_window) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
    glfwTerminate();
  }
  if (m_swsCtx) {
    sws_freeContext(m_swsCtx);
    m_swsCtx = nullptr;
  }
}

void OpenGLVideoOutput::displayFrame(const uint8_t *rgba, int linesize) {
  if (!m_window)
    return;
  glfwMakeContextCurrent(m_window);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, linesize / 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0.f, 1.f);
  glVertex2f(-1.f, -1.f);
  glTexCoord2f(1.f, 1.f);
  glVertex2f(1.f, -1.f);
  glTexCoord2f(1.f, 0.f);
  glVertex2f(1.f, 1.f);
  glTexCoord2f(0.f, 0.f);
  glVertex2f(-1.f, 1.f);
  glEnd();
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

void OpenGLVideoOutput::displayFrame(const VideoFrame &frame) {
  if (!m_swsCtx) {
    m_swsCtx =
        sws_getContext(frame.width, frame.height, AV_PIX_FMT_YUV420P, frame.width, frame.height,
                       AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
  }
  std::vector<uint8_t> rgba(frame.width * frame.height * 4);
  uint8_t *dstData[4] = {rgba.data(), nullptr, nullptr, nullptr};
  int dstLinesize[4] = {frame.width * 4, 0, 0, 0};
  sws_scale(m_swsCtx, frame.data, frame.linesize, 0, frame.height, dstData, dstLinesize);
  displayFrame(rgba.data(), frame.width * 4);
}

} // namespace mediaplayer
