#ifndef MEDIAPLAYER_NULLVIDEOOUTPUT_H
#define MEDIAPLAYER_NULLVIDEOOUTPUT_H

#include "VideoOutput.h"
#include <iostream>

namespace mediaplayer {

class NullVideoOutput : public VideoOutput {
public:
  bool init(int width, int height) override {
    std::cout << "NullVideoOutput init " << width << "x" << height << "\n";
    return true;
  }
  void shutdown() override { std::cout << "NullVideoOutput shutdown\n"; }
  void displayFrame(const uint8_t *, int) override {}
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NULLVIDEOOUTPUT_H
