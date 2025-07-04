#include "VideoOutputQt.h"
#include <QtQml/qqml.h>

extern "C" {
#include <libswscale/swscale.h>
}

using namespace mediaplayer;

VideoOutputQt::VideoOutputQt(QObject *parent) : QObject(parent) {}

VideoOutputQt::~VideoOutputQt() { shutdown(); }

bool VideoOutputQt::init(int width, int height) {
  m_width = width;
  m_height = height;
  m_image = QImage(width, height, QImage::Format_RGBA8888);
  return true;
}

void VideoOutputQt::shutdown() {
  if (m_swsCtx) {
    sws_freeContext(m_swsCtx);
    m_swsCtx = nullptr;
  }
  std::lock_guard<std::mutex> lock(m_mutex);
  m_image = QImage();
}

void VideoOutputQt::displayFrame(const uint8_t *rgba, int linesize) {
  if (!m_width || !m_height)
    return;
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_image.size() != QSize(m_width, m_height))
    m_image = QImage(m_width, m_height, QImage::Format_RGBA8888);
  for (int y = 0; y < m_height; ++y)
    memcpy(m_image.scanLine(y), rgba + y * linesize, m_width * 4);
  emit frameReady();
}

void VideoOutputQt::displayFrame(const VideoFrame &frame) {
  if (!m_swsCtx) {
    m_swsCtx =
        sws_getContext(frame.width, frame.height, AV_PIX_FMT_YUV420P, frame.width, frame.height,
                       AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);
  }
  std::vector<uint8_t> rgba(frame.width * frame.height * 4);
  uint8_t *dstData[4] = {rgba.data(), nullptr, nullptr, nullptr};
  int dstLines[4] = {frame.width * 4, 0, 0, 0};
  sws_scale(m_swsCtx, frame.data, frame.linesize, 0, frame.height, dstData, dstLines);
  displayFrame(rgba.data(), dstLines[0]);
}

QImage VideoOutputQt::currentFrame() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_image;
}

void mediaplayer::registerVideoOutputQtQmlType() {
  qmlRegisterType<VideoOutputQt>("MediaPlayer", 1, 0, "VideoOutputQt");
}
