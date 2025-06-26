#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "playlist_test.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("song1.mp3", "Song1", "Artist", "Album"));
  assert(db.addMedia("song2.mp3", "Song2", "Artist", "Album"));

  assert(db.createPlaylist("fav"));
  assert(db.addToPlaylist("fav", "song1.mp3"));
  assert(db.addToPlaylist("fav", "song2.mp3"));
  auto items = db.playlistItems("fav");
  assert(items.size() == 2 && items[0].path == "song1.mp3");
  assert(db.removeFromPlaylist("fav", "song1.mp3"));
  items = db.playlistItems("fav");
  assert(items.size() == 1 && items[0].path == "song2.mp3");
  assert(db.deletePlaylist("fav"));
  db.close();
  std::remove(dbPath);
  return 0;
}
