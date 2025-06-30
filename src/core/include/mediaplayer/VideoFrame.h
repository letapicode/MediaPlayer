#ifndef MEDIAPLAYER_VIDEOFRAME_H
#define MEDIAPLAYER_VIDEOFRAME_H

#include <cstdint>

namespace mediaplayer {

struct VideoFrame {
  uint8_t *data[3]; // Y, U and V plane pointers
  int linesize[3];  // Line sizes for each plane
  int width{0};     // Frame width
  int height{0};    // Frame height
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_VIDEOFRAME_H
