#include "mediaplayer/MediaPlayer.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: stress_load_test <media-file>\n";
    return 1;
  }
  const int instances = 4;
  std::vector<std::thread> threads;
  for (int i = 0; i < instances; ++i) {
    threads.emplace_back([&]() {
      mediaplayer::MediaPlayer player;
      assert(player.open(argv[1]));
      player.play();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      player.stop();
    });
  }
  for (auto &t : threads)
    t.join();
  std::cout << "Stress test completed\n";
  return 0;
}
