#ifndef MEDIAPLAYER_PACKETQUEUE_H
#define MEDIAPLAYER_PACKETQUEUE_H

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <cstddef>
#include <queue>

namespace mediaplayer {

class PacketQueue {
public:
  explicit PacketQueue(size_t maxSize = 32);
  ~PacketQueue();

  bool push(const AVPacket *pkt);
  bool pop(AVPacket *&pkt);
  void clear();
  size_t size() const;
  bool full() const;

private:
  std::queue<AVPacket *> m_queue;
  size_t m_maxSize{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PACKETQUEUE_H
