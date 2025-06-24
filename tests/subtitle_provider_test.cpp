#include "mediaplayer/SubtitleProvider.h"
#include <cassert>
#include <fstream>

int main() {
  const char *srt =
      "1\n00:00:01,000 --> 00:00:02,000\nHello\n\n2\n00:00:03,000 --> 00:00:04,000\nWorld\n";
  const char *path = "provider.srt";
  std::ofstream out(path);
  out << srt;
  out.close();

  mediaplayer::SubtitleProvider provider;
  assert(provider.load(path));
  assert(provider.subtitleAt(1.5)->text == "Hello");
  assert(provider.subtitleAt(3.5)->text == "World");
  provider.setOffset(-1.0);
  assert(provider.subtitleAt(2.5)->text == "World");
  std::remove(path);
  return 0;
}
