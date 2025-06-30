#include "mediaplayer/FormatConverter.h"
#include <iostream>
#include <string>

using namespace mediaplayer;

static void usage() {
  std::cout << "Usage: mediaconvert <audio|video> <in> <out> [options]\n"
               "Audio options: --bitrate <b> [--sample-rate <sr>] [--codec <name>]\n"
               "Video options: --width <w> --height <h> --bitrate <b> [--codec <name>]\n";
}

int main(int argc, char **argv) {
  if (argc < 4) {
    usage();
    return 1;
  }

  std::string mode = argv[1];
  std::string input = argv[2];
  std::string output = argv[3];

  AudioEncodeOptions aopts;
  VideoEncodeOptions vopts;

  for (int i = 4; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--bitrate" && i + 1 < argc) {
      int value = std::stoi(argv[++i]);
      aopts.bitrate = value;
      vopts.bitrate = value;
    } else if (arg == "--sample-rate" && i + 1 < argc) {
      aopts.sampleRate = std::stoi(argv[++i]);
    } else if (arg == "--width" && i + 1 < argc) {
      vopts.width = std::stoi(argv[++i]);
    } else if (arg == "--height" && i + 1 < argc) {
      vopts.height = std::stoi(argv[++i]);
    } else if (arg == "--codec" && i + 1 < argc) {
      aopts.codec = argv[++i];
      vopts.codec = aopts.codec;
    } else {
      std::cerr << "Unknown option: " << arg << "\n";
      usage();
      return 1;
    }
  }

  FormatConverter conv;
  bool success = false;
  auto progress = [](float p) {
    int pct = static_cast<int>(p * 100);
    std::cout << "\rProgress: " << pct << "%" << std::flush;
  };
  auto done = [&](bool ok) { success = ok; };

  if (mode == "audio") {
    conv.convertAudioAsync(input, output, aopts, progress, done);
  } else if (mode == "video") {
    conv.convertVideoAsync(input, output, vopts, progress, done);
  } else {
    usage();
    return 1;
  }

  conv.wait();
  std::cout << (success ? "\nDone\n" : "\nConversion failed\n");
  return success ? 0 : 1;
}
