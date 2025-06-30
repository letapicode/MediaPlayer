#ifndef MEDIAPLAYER_VIDEOFRAME_H
#define MEDIAPLAYER_VIDEOFRAME_H

#include <cstdint>
#include <vector>

namespace mediaplayer {

struct VideoFrame {
  std::vector<uint8_t> data;
  int width{0};
  int height{0};
  int linesize{0};
  double pts{0.0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOFRAME_H
