#ifndef MEDIAPLAYER_VIDEOFRAME_H
#define MEDIAPLAYER_VIDEOFRAME_H

#include <cstdint>

namespace mediaplayer {

struct VideoFrame {
  uint8_t *data[3]{nullptr, nullptr, nullptr};
  uint8_t *buffer{nullptr};
  int linesize[3]{0, 0, 0};
  int width{0};
  int height{0};
  double pts{0.0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOFRAME_H
