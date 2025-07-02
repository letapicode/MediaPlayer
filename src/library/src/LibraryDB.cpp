#include "mediaplayer/LibraryDB.h"
#include <ctime>
#include <filesystem>
#include <iostream>
#include <libavformat/avformat.h>
#include <mutex>
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
  char *err = nullptr;
  if (sqlite3_exec(m_db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to enable foreign keys: " << err << '\n';
    sqlite3_free(err);
    sqlite3_close(m_db);
    m_db = nullptr;
    return false;
  }
  if (sqlite3_exec(m_db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to set WAL mode: " << err << '\n';
    sqlite3_free(err);
    sqlite3_close(m_db);
    m_db = nullptr;
    return false;
  }
  if (!initSchema()) {
    sqlite3_close(m_db);
    m_db = nullptr;
    return false;
  }
  return true;
}

void LibraryDB::close() {
  if (m_db) {
    sqlite3_close(m_db);
    m_db = nullptr;
    std::error_code ec;
    std::filesystem::remove(m_path + "-wal", ec);
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
                    "rating INTEGER DEFAULT 0,"
                    "play_count INTEGER DEFAULT 0,"
                    "last_played INTEGER"
                    ");"
                    "CREATE TABLE IF NOT EXISTS Playlist ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT UNIQUE"
                    ");"
                    "CREATE TABLE IF NOT EXISTS PlaylistItem ("
                    "playlist_id INTEGER,"
                    "path TEXT,"
                    "position INTEGER,"
                    "FOREIGN KEY(playlist_id) REFERENCES Playlist(id),"
                    "FOREIGN KEY(path) REFERENCES MediaItem(path) ON DELETE CASCADE,"
                    "UNIQUE(playlist_id, path)"
                    ");";
  char *err = nullptr;
  if (sqlite3_exec(m_db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to init schema: " << err << '\n';
    sqlite3_free(err);
    return false;
  }

  const char *indexSql = "CREATE INDEX IF NOT EXISTS idx_media_title ON MediaItem(title);"
                         "CREATE INDEX IF NOT EXISTS idx_media_artist ON MediaItem(artist);"
                         "CREATE INDEX IF NOT EXISTS idx_media_album ON MediaItem(album);";
  if (sqlite3_exec(m_db, indexSql, nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to create indexes: " << err << '\n';
    sqlite3_free(err);
    return false;
  }
  return true;
}

// Insert a media item or update the existing row if the path already exists.
// The SQLite UPSERT ensures rescanning refreshes metadata without a separate
// update step.
bool LibraryDB::insertMedia(const std::string &path, const std::string &title,
                            const std::string &artist, const std::string &album, int duration,
                            int width, int height, int rating) {
  std::lock_guard<std::mutex> lock(m_mutex);
  const char *sql =
      "INSERT INTO MediaItem (path, title, artist, album, duration, width, height, rating) "
      "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8) "
      "ON CONFLICT(path) DO UPDATE SET "
      "title=excluded.title, artist=excluded.artist, album=excluded.album, "
      "duration=excluded.duration, width=excluded.width, height=excluded.height;";
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
  sqlite3_bind_int(stmt, 8, rating);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::scanDirectory(const std::string &directory) {
  return scanDirectoryImpl(directory, nullptr, nullptr);
}

bool LibraryDB::scanDirectoryImpl(const std::string &directory, ProgressCallback progress,
                                  std::atomic<bool> *cancelFlag) {
  namespace fs = std::filesystem;
  if (!m_db)
    return false;

  size_t total = 0;
  for (auto const &entry : fs::recursive_directory_iterator(directory)) {
    if (entry.is_regular_file())
      ++total;
  }

  size_t processed = 0;
  for (auto const &entry : fs::recursive_directory_iterator(directory)) {
    if (cancelFlag && cancelFlag->load())
      break;
    if (!entry.is_regular_file())
      continue;
    auto pathStr = entry.path().string();
    TagLib::FileRef f(pathStr.c_str());
    std::string title;
    std::string artist;
    std::string album;
    bool tagOk = false;
    if (!f.isNull()) {
      tagOk = f.tag() || f.audioProperties();
      if (f.tag()) {
        title = f.tag()->title().to8Bit(true);
        artist = f.tag()->artist().to8Bit(true);
        album = f.tag()->album().to8Bit(true);
      }
    }
    if (title.empty())
      title = entry.path().filename().string();

    int duration = 0;
    int width = 0;
    int height = 0;
    AVFormatContext *ctx = nullptr;
    bool ffOk = false;
    if (avformat_open_input(&ctx, pathStr.c_str(), nullptr, nullptr) == 0) {
      if (avformat_find_stream_info(ctx, nullptr) >= 0) {
        ffOk = true;
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
    if (tagOk || ffOk)
      insertMedia(pathStr, title, artist, album, duration, width, height, 0);
    ++processed;
    if (progress)
      progress(processed, total);
  }
  return true;
}

std::thread LibraryDB::scanDirectoryAsync(const std::string &directory, ProgressCallback progress,
                                          std::atomic<bool> &cancelFlag) {
  return std::thread([this, directory, progress, &cancelFlag]() {
    scanDirectoryImpl(directory, progress, &cancelFlag);
  });
}

bool LibraryDB::addMedia(const std::string &path, const std::string &title,
                         const std::string &artist, const std::string &album) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  return insertMedia(path, title, artist, album, 0, 0, 0, 0);
}

bool LibraryDB::updateMedia(const std::string &path, const std::string &title,
                            const std::string &artist, const std::string &album) {
  std::lock_guard<std::mutex> lock(m_mutex);
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
  std::lock_guard<std::mutex> lock(m_mutex);
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
  std::lock_guard<std::mutex> lock(m_mutex);
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
  std::lock_guard<std::mutex> lock(m_mutex);
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

int LibraryDB::playlistId(const std::string &name) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  const char *sql = "SELECT id FROM Playlist WHERE name=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return -1;
  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  int id = -1;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    id = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return id;
}

bool LibraryDB::createPlaylist(const std::string &name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  const char *sql = "INSERT OR IGNORE INTO Playlist (name) VALUES (?1);";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::deletePlaylist(const std::string &name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  int id = playlistId(name);
  if (id < 0)
    return false;
  const char *sql1 = "DELETE FROM PlaylistItem WHERE playlist_id=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql1, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  const char *sql2 = "DELETE FROM Playlist WHERE id=?1;";
  if (sqlite3_prepare_v2(m_db, sql2, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::addToPlaylist(const std::string &name, const std::string &path) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  int id = playlistId(name);
  if (id < 0)
    return false;
  const char *sql = "INSERT OR IGNORE INTO PlaylistItem (playlist_id, path, position) "
                    "VALUES (?1, ?2, COALESCE((SELECT MAX(position)+1 FROM PlaylistItem WHERE "
                    "playlist_id=?1), 0));";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(stmt);
  bool ok = false;
  if (rc == SQLITE_DONE) {
    ok = sqlite3_changes(m_db) > 0;
  }
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::removeFromPlaylist(const std::string &name, const std::string &path) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  int id = playlistId(name);
  if (id < 0)
    return false;
  const char *sql = "DELETE FROM PlaylistItem WHERE playlist_id=?1 AND path=?2;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_text(stmt, 2, path.c_str(), -1, SQLITE_TRANSIENT);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

std::vector<MediaMetadata> LibraryDB::playlistItems(const std::string &name) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> items;
  if (!m_db)
    return items;
  int id = playlistId(name);
  if (id < 0)
    return items;
  const char *sql = "SELECT m.path,m.title,m.artist,m.album,m.duration,m.width,m.height "
                    "FROM PlaylistItem p JOIN MediaItem m ON p.path=m.path "
                    "WHERE p.playlist_id=?1 ORDER BY p.position;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return items;
  sqlite3_bind_int(stmt, 1, id);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    MediaMetadata m{};
    const unsigned char *txt = sqlite3_column_text(stmt, 0);
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
    items.push_back(std::move(m));
  }
  sqlite3_finalize(stmt);
  return items;
}

bool LibraryDB::setRating(const std::string &path, int rating) {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  if (rating < 0)
    rating = 0;
  if (rating > 5)
    rating = 5;
  const char *sql = "UPDATE MediaItem SET rating=?2 WHERE path=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, rating);
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

int LibraryDB::rating(const std::string &path) const {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return 0;
  const char *sql = "SELECT rating FROM MediaItem WHERE path=?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return 0;
  sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
  int r = 0;
  if (sqlite3_step(stmt) == SQLITE_ROW)
    r = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return r;
}

} // namespace mediaplayer
