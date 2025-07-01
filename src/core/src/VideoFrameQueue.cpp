#include "mediaplayer/VideoFrameQueue.h"

namespace mediaplayer {

VideoFrameQueue::VideoFrameQueue(size_t maxSize) : m_maxSize(maxSize) {}

VideoFrameQueue::~VideoFrameQueue() { clear(); }

bool VideoFrameQueue::push(VideoFrame *frame) {
  if (!frame)
    return false;
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_queue.size() >= m_maxSize)
    return false;
  m_queue.push(frame);
  m_cv.notify_one();
  return true;
}

bool VideoFrameQueue::pop(VideoFrame *&frame, const std::function<bool()> &waitPredicate) {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (waitPredicate) {
    m_cv.wait(lock, [this, &waitPredicate]() { return !m_queue.empty() || waitPredicate(); });
  }
  if (m_queue.empty())
    return false;
  frame = m_queue.front();
  m_queue.pop();
  return true;
}

void VideoFrameQueue::clear() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (!m_queue.empty()) {
    auto *f = m_queue.front();
    delete[] f->buffer;
    delete f;
    m_queue.pop();
  }
  m_cv.notify_all();
}

size_t VideoFrameQueue::size() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.size();
}

bool VideoFrameQueue::full() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.size() >= m_maxSize;
}

} // namespace mediaplayer
