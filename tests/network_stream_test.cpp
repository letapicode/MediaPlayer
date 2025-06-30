#include "mediaplayer/NetworkStream.h"
#include <cassert>

int main() {
  mediaplayer::NetworkStream stream;
  assert(stream.open("http://example.com/sample.mp4") && "open failed");
  assert(stream.context() != nullptr && "context null");
  return 0;
}
