#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>
#include <string>

static int playCount(sqlite3 *db, const std::string &path) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT play_count FROM MediaItem WHERE path=?1;", -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  int count = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    count = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return count;
}

static int lastPlayed(sqlite3 *db, const std::string &path) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT last_played FROM MediaItem WHERE path=?1;", -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  int ts = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    ts = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return ts;
}

int main() {
  const char *dbPath = "playback_test.db";
  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.addMedia("song.mp3", "Title", "Artist", "Album"));
    db.close();
  }
  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  assert(playCount(conn, "song.mp3") == 0);
  sqlite3_close(conn);

  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.recordPlayback("song.mp3"));
  db.close();

  sqlite3_open(dbPath, &conn);
  assert(playCount(conn, "song.mp3") == 1);
  assert(lastPlayed(conn, "song.mp3") > 0);
  sqlite3_close(conn);
  std::remove(dbPath);
  return 0;
}
