#ifndef MEDIAPLAYER_VIDEOFRAMEQUEUE_H
#define MEDIAPLAYER_VIDEOFRAMEQUEUE_H

#include "VideoFrame.h"
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>

namespace mediaplayer {

class VideoFrameQueue {
public:
  explicit VideoFrameQueue(size_t maxSize = 3);
  ~VideoFrameQueue();

  bool push(VideoFrame *frame);
  bool pop(VideoFrame *&frame, const std::function<bool()> &waitPredicate = nullptr);
  void clear();
  size_t size() const;
  bool full() const;

private:
  std::queue<VideoFrame *> m_queue;
  size_t m_maxSize{0};
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOFRAMEQUEUE_H
