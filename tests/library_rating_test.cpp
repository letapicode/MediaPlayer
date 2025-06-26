#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "rating.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("song.mp3", "Title", "Artist", "Album"));
  assert(db.setRating("song.mp3", 4));
  assert(db.rating("song.mp3") == 4);
  assert(db.setRating("song.mp3", 6));
  assert(db.rating("song.mp3") == 5);
  assert(db.setRating("song.mp3", -1));
  assert(db.rating("song.mp3") == 0);
  db.close();
  std::remove(dbPath);
  return 0;
}
