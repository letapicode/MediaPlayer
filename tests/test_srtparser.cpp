#include "mediaplayer/SrtParser.h"
#include <cassert>
#include <fstream>
#include <iostream>

int main() {
  const char *srt =
      "1\n00:00:01,000 --> 00:00:02,000\nHello\n\n2\n00:00:02,500 --> 00:00:04,000\nWorld\n";
  const char *path = "sample.srt";
  std::ofstream out(path);
  out << srt;
  out.close();

  mediaplayer::SrtParser parser;
  assert(parser.load(path));
  assert(parser.cues().size() == 2);
  assert(parser.cueAt(1.5) != nullptr);
  assert(parser.cueAt(1.5)->text == "Hello");
  assert(parser.cueAt(3.0)->text == "World");
  std::remove(path);
  std::cout << "SrtParser test passed\n";
  return 0;
}
