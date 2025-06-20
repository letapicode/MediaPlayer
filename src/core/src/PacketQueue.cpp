#include "mediaplayer/PacketQueue.h"

namespace mediaplayer {

PacketQueue::PacketQueue(size_t maxSize) : m_maxSize(maxSize) {}

PacketQueue::~PacketQueue() { clear(); }

bool PacketQueue::push(const AVPacket *pkt) {
  if (!pkt)
    return false;
  std::unique_lock<std::mutex> lock(m_mutex);
  if (full())
    return false;
  AVPacket *copy = av_packet_alloc();
  if (!copy)
    return false;
  av_packet_ref(copy, pkt);
  m_queue.push(copy);
  m_cv.notify_one();
  return true;
}

bool PacketQueue::pop(AVPacket *&pkt) {
  std::unique_lock<std::mutex> lock(m_mutex);
  if (m_queue.empty())
    return false;
  pkt = m_queue.front();
  m_queue.pop();
  return true;
}

void PacketQueue::clear() {
  std::unique_lock<std::mutex> lock(m_mutex);
  while (!m_queue.empty()) {
    AVPacket *p = m_queue.front();
    av_packet_free(&p);
    m_queue.pop();
  }
}

size_t PacketQueue::size() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.size();
}

bool PacketQueue::full() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_queue.size() >= m_maxSize;
}

} // namespace mediaplayer
