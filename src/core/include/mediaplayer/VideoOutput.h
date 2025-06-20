#ifndef MEDIAPLAYER_VIDEOOUTPUT_H
#define MEDIAPLAYER_VIDEOOUTPUT_H

#include <cstdint>

namespace mediaplayer {

class VideoOutput {
public:
  virtual ~VideoOutput() = default;
  virtual bool init(int width, int height) = 0;
  virtual void shutdown() = 0;
  virtual void displayFrame(const uint8_t *rgba, int linesize) = 0;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOOUTPUT_H
