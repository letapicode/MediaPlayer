#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>
#include <string>

static int countRows(sqlite3 *db) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM MediaItem;", -1, &stmt, nullptr);
  int count = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    count = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return count;
}

static std::string titleFor(sqlite3 *db, const std::string &path) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT title FROM MediaItem WHERE path=?1;", -1, &stmt, nullptr);
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  std::string title;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *txt = sqlite3_column_text(stmt, 0);
    if (txt)
      title = reinterpret_cast<const char *>(txt);
  }
  sqlite3_finalize(stmt);
  return title;
}

int main() {
  const char *dbPath = "test_library.db";
  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.addMedia("song.mp3", "Old", "Artist", "Album"));
    db.close();
  }
  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  assert(countRows(conn) == 1);
  assert(titleFor(conn, "song.mp3") == "Old");
  sqlite3_close(conn);

  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.updateMedia("song.mp3", "New", "Artist", "Album"));
    db.close();
  }
  sqlite3_open(dbPath, &conn);
  assert(titleFor(conn, "song.mp3") == "New");
  sqlite3_close(conn);

  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.removeMedia("song.mp3"));
    db.close();
  }
  sqlite3_open(dbPath, &conn);
  assert(countRows(conn) == 0);
  sqlite3_close(conn);
  std::remove(dbPath);
  return 0;
}
