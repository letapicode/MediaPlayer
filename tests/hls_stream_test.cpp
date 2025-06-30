#include "mediaplayer/HlsStream.h"
#include <cassert>
#include <iostream>

int main() {
  mediaplayer::HlsStream stream;
  assert(stream.open("https://test-streams.mux.dev/x36xhzz/x36xhzz.m3u8"));
  assert(stream.context() != nullptr);
  std::cout << "HlsStream test passed\n";
  return 0;
}
