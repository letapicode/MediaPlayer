#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "smart.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("s1.mp3", "Song1", "Artist", "Album"));
  assert(db.addMedia("s2.mp3", "Song2", "Artist", "Album"));
  assert(db.setRating("s1.mp3", 5));
  assert(db.createSmartPlaylist("high", "rating>=5"));
  auto items = db.playlistItems("high");
  assert(items.size() == 1 && items[0].path == "s1.mp3");
  assert(db.updateSmartPlaylist("high", "artist='Artist'"));
  items = db.playlistItems("high");
  assert(items.size() == 2);
  assert(db.deleteSmartPlaylist("high"));
  db.close();
  std::remove(dbPath);
  return 0;
}
