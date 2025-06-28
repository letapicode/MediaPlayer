#include "mediaplayer/RingBuffer.h"

#include <algorithm>

namespace mediaplayer {

RingBuffer::RingBuffer(size_t capacity) {
  m_size = capacity + 1; // one slot unused
  m_buffer.resize(m_size);
  m_head.store(0);
  m_tail.store(0);
}

size_t RingBuffer::increment(size_t idx, size_t val) const { return (idx + val) % m_size; }

size_t RingBuffer::available() const {
  size_t head = m_head.load(std::memory_order_acquire);
  size_t tail = m_tail.load(std::memory_order_acquire);
  if (tail >= head)
    return tail - head;
  return m_size - head + tail;
}

size_t RingBuffer::freeSpace() const { return m_size - available() - 1; }

void RingBuffer::clear() {
  m_head.store(0, std::memory_order_release);
  m_tail.store(0, std::memory_order_release);
  std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

size_t RingBuffer::write(const int16_t *data, size_t samples) {
  size_t space = freeSpace();
  size_t toWrite = std::min(samples, space);
  size_t tail = m_tail.load(std::memory_order_relaxed);
  for (size_t i = 0; i < toWrite; ++i) {
    m_buffer[tail] = data[i];
    tail = increment(tail, 1);
  }
  m_tail.store(tail, std::memory_order_release);
  return toWrite;
}

size_t RingBuffer::mix(const int16_t *data, size_t samples) {
  std::lock_guard<std::mutex> lock(m_mixMutex);
  size_t space = freeSpace();
  size_t toWrite = std::min(samples, space);
  size_t tail = m_tail.load(std::memory_order_relaxed);
  for (size_t i = 0; i < toWrite; ++i) {
    int32_t sum = static_cast<int32_t>(m_buffer[tail]) + data[i];
    if (sum < -32768)
      sum = -32768;
    else if (sum > 32767)
      sum = 32767;
    m_buffer[tail] = static_cast<int16_t>(sum);
    tail = increment(tail, 1);
  }
  m_tail.store(tail, std::memory_order_release);
  return toWrite;
}

size_t RingBuffer::read(int16_t *out, size_t samples) {
  size_t avail = available();
  size_t toRead = std::min(samples, avail);
  size_t head = m_head.load(std::memory_order_relaxed);
  for (size_t i = 0; i < toRead; ++i) {
    out[i] = m_buffer[head];
    m_buffer[head] = 0; // clear after reading for mixing
    head = increment(head, 1);
  }
  m_head.store(head, std::memory_order_release);
  return toRead;
}

} // namespace mediaplayer
