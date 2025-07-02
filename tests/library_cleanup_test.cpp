#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>

static int countRows(sqlite3 *db) {
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM MediaItem;", -1, &stmt, nullptr);
  int count = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    count = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return count;
}

int main() {
  const char *dbPath = "cleanup.db";
  {
    mediaplayer::LibraryDB db(dbPath);
    assert(db.open());
    assert(db.addMedia("ghost.mp3", "Ghost", "", ""));
    db.close();
  }

  mediaplayer::LibraryDB db2(dbPath);
  assert(db2.open());
  db2.scanDirectory(".");
  db2.close();

  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  assert(countRows(conn) == 0);
  sqlite3_close(conn);
  std::remove(dbPath);
  return 0;
}
