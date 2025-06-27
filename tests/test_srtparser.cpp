#include "mediaplayer/SrtParser.h"
#include <cassert>
#include <fstream>
#include <iostream>

int main() {
  // Basic single-line cues
  const char *srt1 =
      "1\n00:00:01,000 --> 00:00:02,000\nHello\n\n2\n00:00:02,500 --> 00:00:04,000\nWorld\n";
  const char *path1 = "sample.srt";
  std::ofstream out1(path1);
  out1 << srt1;
  out1.close();

  mediaplayer::SrtParser parser;
  assert(parser.load(path1));
  assert(parser.cues().size() == 2);
  assert(parser.cueAt(1.5) != nullptr);
  assert(parser.cueAt(1.5)->text == "Hello");
  assert(parser.cueAt(3.0)->text == "World");
  std::remove(path1);

  // Multi-line cue parsing
  const char *srt2 = "1\n00:00:05,000 --> 00:00:07,000\nLine1\nLine2\n\n";
  const char *path2 = "multiline.srt";
  std::ofstream out2(path2);
  out2 << srt2;
  out2.close();

  parser = mediaplayer::SrtParser();
  assert(parser.load(path2));
  assert(parser.cues().size() == 1);
  assert(parser.cueAt(6.0)->text == std::string("Line1\nLine2"));
  std::remove(path2);

  // Invalid timestamp format should fail to load
  const char *srt3 = "1\n00:00:01,000 -> 00:00:02,000\nBad\n";
  const char *path3 = "invalid.srt";
  std::ofstream out3(path3);
  out3 << srt3;
  out3.close();

  parser = mediaplayer::SrtParser();
  assert(!parser.load(path3));
  std::remove(path3);

  // Overlapping cues and out-of-range query
  const char *srt4 =
      "1\n00:00:01,000 --> 00:00:04,000\nFirst\n\n2\n00:00:03,000 --> 00:00:05,000\nSecond\n";
  const char *path4 = "overlap.srt";
  std::ofstream out4(path4);
  out4 << srt4;
  out4.close();

  parser = mediaplayer::SrtParser();
  assert(parser.load(path4));
  assert(parser.cues().size() == 2);
  assert(parser.cueAt(3.5)->text == "First");
  assert(parser.cueAt(6.0) == nullptr);
  std::remove(path4);

  std::cout << "SrtParser test passed\n";
  return 0;
}
