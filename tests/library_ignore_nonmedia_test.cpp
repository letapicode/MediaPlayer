#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>
#include <sqlite3.h>
#include <sys/stat.h>

int main() {
  const char *dir = "nonmedia_dir";
  mkdir(dir, 0755);
  const std::string file = std::string(dir) + "/dummy.txt";
  FILE *f = fopen(file.c_str(), "w");
  fputs("hello", f);
  fclose(f);

  const char *dbPath = "ignore_nonmedia.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  db.scanDirectory(dir);
  db.close();

  sqlite3 *conn = nullptr;
  sqlite3_open(dbPath, &conn);
  sqlite3_stmt *stmt = nullptr;
  sqlite3_prepare_v2(conn, "SELECT COUNT(*) FROM MediaItem;", -1, &stmt, nullptr);
  bool empty = false;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    empty = sqlite3_column_int(stmt, 0) == 0;
  sqlite3_finalize(stmt);
  sqlite3_close(conn);

  std::remove(file.c_str());
  rmdir(dir);
  std::remove(dbPath);
  assert(empty && "non-media files should be ignored");
  return 0;
}
