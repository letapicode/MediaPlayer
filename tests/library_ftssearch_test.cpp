#include "mediaplayer/LibraryDB.h"
#include <cassert>
#include <cstdio>

int main() {
  const char *dbPath = "fts_search_test.db";
  mediaplayer::LibraryDB db(dbPath);
  assert(db.open());
  assert(db.addMedia("foo.mp3", "Hello World", "Artist1", "Album"));
  assert(db.addMedia("bar.mp3", "Another Song", "Artist2", "Album"));
  assert(db.addMedia("baz.mp3", "Nothing", "Artist3", "Album"));

  auto res = db.searchFts("hello");
  assert(res.size() == 1 && res[0].path == "foo.mp3");

  // FTS supports prefix query with *
  auto res2 = db.searchFts("arti* AND song");
  assert(res2.size() == 1 && res2[0].path == "bar.mp3");

  db.close();
  std::remove(dbPath);
  return 0;
}
