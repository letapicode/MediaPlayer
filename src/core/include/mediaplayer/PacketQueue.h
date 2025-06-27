#ifndef MEDIAPLAYER_PACKETQUEUE_H
#define MEDIAPLAYER_PACKETQUEUE_H

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>

namespace mediaplayer {

class PacketQueue {
public:
  explicit PacketQueue(size_t maxSize = 32);
  ~PacketQueue();

  bool push(const AVPacket *pkt);
  bool pop(AVPacket *&pkt, const std::function<bool()> &waitPredicate = [] { return false; });
  void clear();
  size_t size() const;
  bool full() const;

private:
  std::queue<AVPacket *> m_queue;
  size_t m_maxSize{0};
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PACKETQUEUE_H
