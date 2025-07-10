#include "mediaplayer/MediaPlayer.h"
#include "mediaplayer/NullAudioOutput.h"
#include "mediaplayer/NullVideoOutput.h"
#include <iostream>

using mediaplayer::MediaPlayer;
using mediaplayer::NullAudioOutput;
using mediaplayer::NullVideoOutput;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: format_probe <media_file>" << std::endl;
    return 1;
  }
  MediaPlayer player;
  player.setAudioOutput(std::make_unique<NullAudioOutput>());
  player.setVideoOutput(std::make_unique<NullVideoOutput>());
  if (!player.open(argv[1])) {
    std::cerr << "Failed to open" << std::endl;
    return 1;
  }
  std::cout << "Opened successfully" << std::endl;
  return 0;
}
