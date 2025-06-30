#ifndef MEDIAPLAYER_VIDEOFRAME_H
#define MEDIAPLAYER_VIDEOFRAME_H

#include <cstdint>

namespace mediaplayer {

struct VideoFrame {
  const uint8_t *planes[3]{nullptr, nullptr, nullptr};
  int linesize[3]{0, 0, 0};
  int width{0};
  int height{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOFRAME_H
