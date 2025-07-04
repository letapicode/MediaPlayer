#include "VideoItem.h"
#include "../VideoOutputQt.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <memory>

using namespace mediaplayer;

class VideoItemRenderer : public QQuickFramebufferObject::Renderer, protected QOpenGLFunctions {
public:
  explicit VideoItemRenderer(VideoItem *item) : m_item(item) { initializeOpenGLFunctions(); }

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

    if (!m_program) {
      m_program = std::make_unique<QOpenGLShaderProgram>();
      const char *vsrc = "attribute vec2 vertex;"
                         "attribute vec2 texCoord;"
                         "varying vec2 vTex;"
                         "void main() { vTex = texCoord; gl_Position = vec4(vertex, 0.0, 1.0); }";
      const char *fsrc = "varying highp vec2 vTex;"
                         "uniform sampler2D frame;"
                         "void main() { gl_FragColor = texture2D(frame, vTex); }";
      m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
      m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
      m_program->link();
    }

    if (!m_texture) {
      glGenTextures(1, &m_texture);
      glBindTexture(GL_TEXTURE_2D, m_texture);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, m_texture);
    if (img.size() != m_texSize) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, img.constBits());
      m_texSize = img.size();
    } else {
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                      img.constBits());
    }

    GLfloat verts[] = {-1.f, -1.f, 1.f, -1.f, -1.f, 1.f, 1.f, 1.f};
    GLfloat tex[] = {0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f};

    glViewport(0, 0, framebufferObject()->width(), framebufferObject()->height());
    m_program->bind();
    m_program->setUniformValue("frame", 0);

    int loc = m_program->attributeLocation("vertex");
    m_program->enableAttributeArray(loc);
    m_program->setAttributeArray(loc, GL_FLOAT, verts, 2);

    int tloc = m_program->attributeLocation("texCoord");
    m_program->enableAttributeArray(tloc);
    m_program->setAttributeArray(tloc, GL_FLOAT, tex, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(loc);
    m_program->disableAttributeArray(tloc);
    m_program->release();
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  ~VideoItemRenderer() override {
    if (m_texture)
      glDeleteTextures(1, &m_texture);
  }

private:
  VideoItem *m_item{nullptr};
  VideoOutputQt *m_output{nullptr};
  GLuint m_texture{0};
  QSize m_texSize;
  std::unique_ptr<QOpenGLShaderProgram> m_program;
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
