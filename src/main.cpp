#include "core/include/MediaPlayer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    return 0;
  }
  mediaplayer::MediaPlayer player;
  player.open(argv[1]);
  player.play();
  return 0;
}
