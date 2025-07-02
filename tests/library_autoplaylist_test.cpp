#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>

int main() {
  const char *dbPath = "autoplaylist.db";
  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.addMedia("one.mp3", "One", "A", "Al"));
    assert(db.addMedia("two.mp3", "Two", "A", "Al"));
    assert(db.addMedia("three.mp3", "Three", "A", "Al"));
    db.close();
  }

  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=100, play_count=5 WHERE path='one.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=200, play_count=2 WHERE path='two.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_exec(conn, "UPDATE MediaItem SET added_date=300, play_count=10 WHERE path='three.mp3';",
               nullptr, nullptr, nullptr);
  sqlite3_close(conn);

  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  auto recent = db.recentlyAdded(3);
  assert(recent.size() == 3);
  assert(recent[0].path == "three.mp3");
  assert(recent[1].path == "two.mp3");
  assert(recent[2].path == "one.mp3");

  auto popular = db.mostPlayed(2);
  assert(popular.size() == 2);
  assert(popular[0].path == "three.mp3");
  assert(popular[1].path == "one.mp3");

  db.close();
  std::remove(dbPath);
  return 0;
}
