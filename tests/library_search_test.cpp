#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "search_test.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("song1.mp3", "Hello World", "Artist1", "Album1"));
  assert(db.addMedia("song2.mp3", "Goodbye", "Artist2", "Album2"));
  assert(db.addMedia("song3.mp3", "Hello Again", "Artist3", "Album3"));

  auto resHello = db.search("Hello");
  assert(resHello.size() == 2);
  auto resArtist = db.search("Artist2");
  assert(resArtist.size() == 1 && resArtist[0].path == "song2.mp3");

  db.close();
  std::remove(dbPath);
  return 0;
}
