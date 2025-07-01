#include "mediaplayer/FramePool.h"
#include <algorithm>
#include <cstring>

namespace mediaplayer {

FramePool::FramePool(size_t maxFrames) : m_maxFrames(maxFrames) {}

FramePool::~FramePool() { clear(); }

VideoFrame *FramePool::createFrame(int width, int height, const int linesize[3]) {
  auto *f = new VideoFrame();
  f->width = width;
  f->height = height;
  f->linesize[0] = linesize[0];
  f->linesize[1] = linesize[1];
  f->linesize[2] = linesize[2];
  int yBytes = linesize[0] * height;
  int uBytes = linesize[1] * height / 2;
  int vBytes = linesize[2] * height / 2;
  f->data[0] = new uint8_t[yBytes];
  f->data[1] = new uint8_t[uBytes];
  f->data[2] = new uint8_t[vBytes];
  return f;
}

void FramePool::preallocate(int width, int height, const int linesize[3], size_t count) {
  std::lock_guard<std::mutex> lock(m_mutex);
  for (size_t i = m_freeFrames.size(); i < count && i < m_maxFrames; ++i) {
    m_freeFrames.push_back(createFrame(width, height, linesize));
  }
}

VideoFrame *FramePool::acquire(int width, int height, const int linesize[3]) {
  std::lock_guard<std::mutex> lock(m_mutex);
  for (auto it = m_freeFrames.begin(); it != m_freeFrames.end(); ++it) {
    VideoFrame *f = *it;
    if (f->width == width && f->height == height && f->linesize[0] == linesize[0] &&
        f->linesize[1] == linesize[1] && f->linesize[2] == linesize[2]) {
      m_freeFrames.erase(it);
      return f;
    }
  }
  return createFrame(width, height, linesize);
}

void FramePool::release(VideoFrame *frame) {
  if (!frame)
    return;
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_freeFrames.size() >= m_maxFrames) {
    delete[] frame->data[0];
    delete[] frame->data[1];
    delete[] frame->data[2];
    delete frame;
  } else {
    m_freeFrames.push_back(frame);
  }
}

void FramePool::clear() {
  std::lock_guard<std::mutex> lock(m_mutex);
  for (auto *f : m_freeFrames) {
    delete[] f->data[0];
    delete[] f->data[1];
    delete[] f->data[2];
    delete f;
  }
  m_freeFrames.clear();
}

} // namespace mediaplayer
