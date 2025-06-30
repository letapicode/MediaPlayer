#include "mediaplayer/OpenGLVideoOutput.h"

#include <GL/gl.h>
#include <fstream>
#include <iostream>

namespace mediaplayer {

static std::string loadFile(const char *path) {
  std::ifstream f(path);
  return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

static GLuint compileShader(GLenum type, const std::string &src) {
  GLuint shader = glCreateShader(type);
  const char *cstr = src.c_str();
  glShaderSource(shader, 1, &cstr, nullptr);
  glCompileShader(shader);
  GLint status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    char log[512];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    std::cerr << "Shader compile error: " << log << std::endl;
  }
  return shader;
}

OpenGLVideoOutput::OpenGLVideoOutput() = default;

OpenGLVideoOutput::~OpenGLVideoOutput() { shutdown(); }

bool OpenGLVideoOutput::init(int width, int height, void *externalContext) {
  if (externalContext) {
    m_window = static_cast<GLFWwindow *>(externalContext);
    m_external = true;
  } else {
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
  }
  glfwMakeContextCurrent(m_window);

  std::string vertSrc = loadFile("yuv.vert");
  std::string fragSrc = loadFile("yuv.frag");
  GLuint vs = compileShader(GL_VERTEX_SHADER, vertSrc);
  GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
  m_program = glCreateProgram();
  glAttachShader(m_program, vs);
  glAttachShader(m_program, fs);
  glLinkProgram(m_program);
  glDeleteShader(vs);
  glDeleteShader(fs);

  glGenTextures(1, &m_texY);
  glBindTexture(GL_TEXTURE_2D, m_texY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &m_texU);
  glBindTexture(GL_TEXTURE_2D, m_texU);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenTextures(1, &m_texV);
  glBindTexture(GL_TEXTURE_2D, m_texV);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_width = width;
  m_height = height;
  return true;
}

void OpenGLVideoOutput::shutdown() {
  if (m_texY) {
    glDeleteTextures(1, &m_texY);
    m_texY = 0;
  }
  if (m_texU) {
    glDeleteTextures(1, &m_texU);
    m_texU = 0;
  }
  if (m_texV) {
    glDeleteTextures(1, &m_texV);
    m_texV = 0;
  }
  if (m_program) {
    glDeleteProgram(m_program);
    m_program = 0;
  }
  if (m_window && !m_external) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
    glfwTerminate();
  }
}

void OpenGLVideoOutput::displayFrame(const VideoFrame &frame) {
  if (!m_window)
    return;
  glfwMakeContextCurrent(m_window);

  glUseProgram(m_program);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texY);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.linesize[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width, frame.height, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, frame.planes[0]);
  glUniform1i(glGetUniformLocation(m_program, "texY"), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_texU);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.linesize[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width / 2, frame.height / 2, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, frame.planes[1]);
  glUniform1i(glGetUniformLocation(m_program, "texU"), 1);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, m_texV);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.linesize[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, frame.width / 2, frame.height / 2, 0, GL_LUMINANCE,
               GL_UNSIGNED_BYTE, frame.planes[2]);
  glUniform1i(glGetUniformLocation(m_program, "texV"), 2);

  glClear(GL_COLOR_BUFFER_BIT);
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

} // namespace mediaplayer
