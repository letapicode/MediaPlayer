#include "mediaplayer/LibraryDB.h"
#include <filesystem>
#include <iostream>
#include <taglib/fileref.h>
#include <taglib/tag.h>

namespace mediaplayer {

LibraryDB::LibraryDB(const std::string &path) : m_path(path) {}

LibraryDB::~LibraryDB() { close(); }

bool LibraryDB::open() {
  if (sqlite3_open(m_path.c_str(), &m_db) != SQLITE_OK) {
    std::cerr << "Failed to open DB: " << sqlite3_errmsg(m_db) << '\n';
    return false;
  }
  return initSchema();
}

void LibraryDB::close() {
  if (m_db) {
    sqlite3_close(m_db);
    m_db = nullptr;
  }
}

bool LibraryDB::initSchema() {
  const char *sql = "CREATE TABLE IF NOT EXISTS MediaItem ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "path TEXT UNIQUE,"
                    "title TEXT,"
                    "artist TEXT,"
                    "album TEXT,"
                    "play_count INTEGER DEFAULT 0,"
                    "last_played INTEGER"
                    ");";
  char *err = nullptr;
  if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to init schema: " << err << '\n';
    sqlite3_free(err);
    return false;
  }
  return true;
}

bool LibraryDB::insertMedia(const std::string &path, const std::string &title,
                            const std::string &artist, const std::string &album) {
  const char *sql = "INSERT OR IGNORE INTO MediaItem (path, title, artist, album)"
                    " VALUES (?1, ?2, ?3, ?4);";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return false;
  }
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, artist.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, album.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::scanDirectory(const std::string &directory) {
  namespace fs = std::filesystem;
  if (!m_db)
    return false;
  for (auto const &entry : fs::recursive_directory_iterator(directory)) {
    if (!entry.is_regular_file())
      continue;
    auto pathStr = entry.path().string();
    TagLib::FileRef f(pathStr.c_str());
    if (!f.isNull() && f.tag()) {
      std::string title = f.tag()->title().to8Bit(true);
      std::string artist = f.tag()->artist().to8Bit(true);
      std::string album = f.tag()->album().to8Bit(true);
      if (title.empty())
        title = entry.path().filename().string();
      insertMedia(pathStr, title, artist, album);
    }
  }
  return true;
}

} // namespace mediaplayer
