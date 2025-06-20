#include "mediaplayer/PacketQueue.h"

namespace mediaplayer {

PacketQueue::PacketQueue(size_t maxSize) : m_maxSize(maxSize) {}

PacketQueue::~PacketQueue() { clear(); }

bool PacketQueue::push(const AVPacket *pkt) {
  if (!pkt || full())
    return false;
  AVPacket *copy = av_packet_alloc();
  if (!copy)
    return false;
  av_packet_ref(copy, pkt);
  m_queue.push(copy);
  return true;
}

bool PacketQueue::pop(AVPacket *&pkt) {
  if (m_queue.empty())
    return false;
  pkt = m_queue.front();
  m_queue.pop();
  return true;
}

void PacketQueue::clear() {
  while (!m_queue.empty()) {
    AVPacket *p = m_queue.front();
    av_packet_free(&p);
    m_queue.pop();
  }
}

size_t PacketQueue::size() const { return m_queue.size(); }

bool PacketQueue::full() const { return m_queue.size() >= m_maxSize; }

} // namespace mediaplayer
