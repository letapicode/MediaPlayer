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
  assert(db.addMedia("song4.mp3", u8"こんにちは", "Artist4", "Album4"));

  auto resHello = db.search("Hello");
  assert(resHello.size() == 2);
  auto resLower = db.search("hello");
  assert(resLower.size() == 2);
  auto resArtist = db.search("Artist2");
  assert(resArtist.size() == 1 && resArtist[0].path == "song2.mp3");
  auto resUnicode = db.search(u8"こんにちは");
  assert(resUnicode.size() == 1 && resUnicode[0].path == "song4.mp3");

  db.close();
  std::remove(dbPath);
  return 0;
}
