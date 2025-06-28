#ifndef MEDIAPLAYER_RINGBUFFER_H
#define MEDIAPLAYER_RINGBUFFER_H

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

namespace mediaplayer {

class RingBuffer {
public:
  explicit RingBuffer(size_t capacity = 16384);
  size_t write(const int16_t *data, size_t samples);
  size_t mix(const int16_t *data, size_t samples);
  size_t read(int16_t *out, size_t samples);
  void clear();
  size_t available() const;
  size_t freeSpace() const;

private:
  size_t increment(size_t idx, size_t val) const;

  std::vector<int16_t> m_buffer;
  size_t m_size{0};
  std::atomic<size_t> m_head{0};
  std::atomic<size_t> m_tail{0};
  mutable std::mutex m_mixMutex;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_RINGBUFFER_H
