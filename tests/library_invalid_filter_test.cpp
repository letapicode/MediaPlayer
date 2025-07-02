#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "invalid_filter.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("song.mp3", "Song", "Artist", "Album"));
  auto res = db.smartQuery("rating>=foo");
  assert(res.empty());
  db.close();
  std::remove(dbPath);
  return 0;
}
