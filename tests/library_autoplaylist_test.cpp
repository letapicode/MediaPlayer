#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>

int main() {
  const char *dbPath = "autoplaylist.db";
  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.addMedia("a.mp3", "A", "Artist", "Album"));
    assert(db.addMedia("b.mp3", "B", "Artist", "Album"));
    assert(db.addMedia("c.mp3", "C", "Artist", "Album"));
    db.close();
  }

  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=1000, play_count=10 WHERE path='a.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=5000, play_count=5 WHERE path='b.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=2000, play_count=8 WHERE path='c.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_close(conn);

  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  auto recent = db.recentlyAdded(3);
  assert(recent.size() == 3);
  assert(recent[0].path == "b.mp3");
  assert(recent[1].path == "c.mp3");
  assert(recent[2].path == "a.mp3");
  auto popular = db.mostPlayed(3);
  assert(popular.size() == 3);
  assert(popular[0].path == "a.mp3");
  assert(popular[1].path == "c.mp3");
  assert(popular[2].path == "b.mp3");
  db.close();

  std::remove(dbPath);
  return 0;
}
