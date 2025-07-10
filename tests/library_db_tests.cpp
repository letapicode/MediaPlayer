#include "mediaplayer/LibraryDB.h"
#include <cassert>

int main() {
  mediaplayer::LibraryDB db(":memory:");
  assert(db.open());

  assert(db.addMedia("t1.mp3", "First", "BandA", "AlbumA"));
  assert(db.addMedia("t2.mp3", "Second", "BandB", "AlbumB"));
  assert(db.setRating("t2.mp3", 4));

  auto results = db.search("Second");
  assert(results.size() == 1 && results[0].path == "t2.mp3");

  assert(db.createSmartPlaylist("rated", "rating>=4 AND artist='BandB'"));
  auto items = db.playlistItems("rated");
  assert(items.size() == 1 && items[0].path == "t2.mp3");

  db.close();
  return 0;
}
