#ifndef MEDIAPLAYER_FRAMEPOOL_H
#define MEDIAPLAYER_FRAMEPOOL_H

#include "VideoFrame.h"
#include <mutex>
#include <vector>

namespace mediaplayer {

class FramePool {
public:
  explicit FramePool(size_t maxFrames = 5);
  ~FramePool();

  VideoFrame *acquire(int width, int height, const int linesize[3]);
  void release(VideoFrame *frame);
  void preallocate(int width, int height, const int linesize[3], size_t count);
  void clear();

private:
  VideoFrame *createFrame(int width, int height, const int linesize[3]);

  size_t m_maxFrames;
  std::vector<VideoFrame *> m_freeFrames;
  std::mutex m_mutex;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_FRAMEPOOL_H
