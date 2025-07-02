#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "smart_query.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("song1.mp3", "Title1", "Artist", "Album"));
  assert(db.addMedia("song2.mp3", "Title2", "Artist", "Album"));
  assert(db.setRating("song1.mp3", 5));
  assert(db.setRating("song2.mp3", 3));

  auto res = db.smartQuery("rating>=4 AND artist='Artist'");
  assert(res.size() == 1 && res[0].path == "song1.mp3");

  db.close();
  std::remove(dbPath);
  return 0;
}
