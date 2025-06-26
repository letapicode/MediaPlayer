#include "mediaplayer/LibraryDB.h"
#include <filesystem>
#include <iostream>
#include <libavformat/avformat.h>
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
                    "duration INTEGER DEFAULT 0,"
                    "width INTEGER DEFAULT 0,"
                    "height INTEGER DEFAULT 0,"
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
                            const std::string &artist, const std::string &album, int duration,
                            int width, int height) {
  const char *sql =
      "INSERT OR IGNORE INTO MediaItem (path, title, artist, album, duration, width, height)"
      " VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7);";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    return false;
  }
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, artist.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, album.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 5, duration);
  sqlite3_bind_int(stmt, 6, width);
  sqlite3_bind_int(stmt, 7, height);
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

      int duration = 0;
      int width = 0;
      int height = 0;
      AVFormatContext *ctx = nullptr;
      if (avformat_open_input(&ctx, pathStr.c_str(), nullptr, nullptr) == 0) {
        if (avformat_find_stream_info(ctx, nullptr) >= 0) {
          if (ctx->duration > 0)
            duration = static_cast<int>(ctx->duration / AV_TIME_BASE);
          for (unsigned i = 0; i < ctx->nb_streams; ++i) {
            AVStream *st = ctx->streams[i];
            if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
              width = st->codecpar->width;
              height = st->codecpar->height;
              break;
            }
          }
        }
        avformat_close_input(&ctx);
      }
      insertMedia(pathStr, title, artist, album, duration, width, height);
    }
  }
  return true;
}

bool LibraryDB::addMedia(const std::string &path, const std::string &title,
                         const std::string &artist, const std::string &album) {
  if (!m_db)
    return false;
  return insertMedia(path, title, artist, album, 0, 0, 0);
}

bool LibraryDB::updateMedia(const std::string &path, const std::string &title,
                            const std::string &artist, const std::string &album) {
  if (!m_db)
    return false;
  const char *sql = "UPDATE MediaItem SET title=?2, artist=?3, album=?4 WHERE path=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, artist.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, album.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::removeMedia(const std::string &path) {
  if (!m_db)
    return false;
  const char *sql = "DELETE FROM MediaItem WHERE path=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

std::vector<MediaMetadata> LibraryDB::search(const std::string &query) {
  std::vector<MediaMetadata> results;
  if (!m_db)
    return results;
  std::string pattern = "%" + query + "%";
  const char *sql = "SELECT path,title,artist,album,duration,width,height FROM MediaItem "
                    "WHERE title LIKE ?1 OR artist LIKE ?1 OR album LIKE ?1 ORDER BY title;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return results;
  sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    MediaMetadata m{};
    const unsigned char *txt = nullptr;
    txt = sqlite3_column_text(stmt, 0);
    if (txt)
      m.path = reinterpret_cast<const char *>(txt);
    txt = sqlite3_column_text(stmt, 1);
    if (txt)
      m.title = reinterpret_cast<const char *>(txt);
    txt = sqlite3_column_text(stmt, 2);
    if (txt)
      m.artist = reinterpret_cast<const char *>(txt);
    txt = sqlite3_column_text(stmt, 3);
    if (txt)
      m.album = reinterpret_cast<const char *>(txt);
    m.duration = sqlite3_column_int(stmt, 4);
    m.width = sqlite3_column_int(stmt, 5);
    m.height = sqlite3_column_int(stmt, 6);
    results.push_back(std::move(m));
  }
  sqlite3_finalize(stmt);
  return results;
}

bool LibraryDB::recordPlayback(const std::string &path) {
  if (!m_db)
    return false;
  const char *sql =
      "UPDATE MediaItem SET play_count = play_count + 1, last_played = ?2 WHERE path=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(time(nullptr)));
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

} // namespace mediaplayer
