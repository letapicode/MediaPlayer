#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/AIRecommender.h"
#include <ctime>
#include <filesystem>
#include <iostream>
#include <libavformat/avformat.h>
#include <mutex>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <unordered_set>

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
                    "added_date INTEGER DEFAULT 0,"
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
                    ");"
                    "CREATE TABLE IF NOT EXISTS SmartPlaylist ("
                    "playlist_id INTEGER PRIMARY KEY,"
                    "definition TEXT,"
                    "FOREIGN KEY(playlist_id) REFERENCES Playlist(id) ON DELETE CASCADE"
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

  const char *alterSql = "ALTER TABLE MediaItem ADD COLUMN added_date INTEGER DEFAULT 0;";
  if (sqlite3_exec(m_db, alterSql, nullptr, nullptr, &err) != SQLITE_OK) {
    std::string msg = err ? err : "";
    if (msg.find("duplicate column name") == std::string::npos) {
      std::cerr << "Failed to alter table: " << msg << '\n';
      sqlite3_free(err);
      return false;
    }
    sqlite3_free(err);
  }

  const char *ftsSql =
      "CREATE VIRTUAL TABLE IF NOT EXISTS MediaItemFTS USING fts5(path,title,artist,album);"
      "CREATE TRIGGER IF NOT EXISTS mediaitem_ai AFTER INSERT ON MediaItem BEGIN "
      "INSERT INTO MediaItemFTS(rowid,path,title,artist,album) "
      "VALUES(new.id,new.path,new.title,new.artist,new.album); END;"
      "CREATE TRIGGER IF NOT EXISTS mediaitem_ad AFTER DELETE ON MediaItem BEGIN "
      "INSERT INTO MediaItemFTS(MediaItemFTS,rowid,path,title,artist,album) "
      "VALUES('delete',old.id,old.path,old.title,old.artist,old.album); END;"
      "CREATE TRIGGER IF NOT EXISTS mediaitem_au AFTER UPDATE ON MediaItem BEGIN "
      "INSERT INTO MediaItemFTS(MediaItemFTS,rowid,path,title,artist,album) "
      "VALUES('delete',old.id,old.path,old.title,old.artist,old.album);"
      "INSERT INTO MediaItemFTS(rowid,path,title,artist,album) "
      "VALUES(new.id,new.path,new.title,new.artist,new.album); END;";
  if (sqlite3_exec(m_db, ftsSql, nullptr, nullptr, &err) != SQLITE_OK) {
    std::cerr << "Failed to create FTS table: " << err << '\n';
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
  const char *sql = "INSERT INTO MediaItem (path, title, artist, album, duration, width, height, "
                    "rating, added_date) "
                    "VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9) "
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
  sqlite3_bind_int64(stmt, 9, static_cast<sqlite3_int64>(time(nullptr)));
  bool ok = sqlite3_step(stmt) == SQLITE_DONE;
  sqlite3_finalize(stmt);
  return ok;
}

bool LibraryDB::scanDirectory(const std::string &directory, bool cleanup) {
  return scanDirectoryImpl(directory, nullptr, nullptr, cleanup);
}

bool LibraryDB::scanDirectoryImpl(const std::string &directory, ProgressCallback progress,
                                  std::atomic<bool> *cancelFlag, bool cleanup) {
  namespace fs = std::filesystem;
  if (!m_db)
    return false;

  std::unordered_set<std::string> seen;

  size_t total = 0;
  try {
    std::error_code ec;
    fs::recursive_directory_iterator it(directory, fs::directory_options::skip_permission_denied,
                                        ec);
    fs::recursive_directory_iterator end;
    for (; it != end; it.increment(ec)) {
      if (ec) {
        std::cerr << "Directory iteration error: " << ec.message() << '\n';
        ec.clear();
        continue;
      }
      try {
        if (it->is_regular_file())
          ++total;
      } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
      }
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  }

  size_t processed = 0;
  try {
    std::error_code ec;
    fs::recursive_directory_iterator it(directory, fs::directory_options::skip_permission_denied,
                                        ec);
    fs::recursive_directory_iterator end;
    for (; it != end; it.increment(ec)) {
      if (ec) {
        std::cerr << "Directory iteration error: " << ec.message() << '\n';
        ec.clear();
        continue;
      }
      if (cancelFlag && cancelFlag->load())
        break;
      try {
        if (!it->is_regular_file())
          continue;
      } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        continue;
      }

      auto pathStr = it->path().string();
      seen.insert(pathStr);
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
        title = it->path().filename().string();

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
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << '\n';
  }

  if (cleanup) {
    std::vector<std::string> dbPaths;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      const char *sql = "SELECT path FROM MediaItem;";
      sqlite3_stmt *stmt = nullptr;
      if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
          const unsigned char *txt = sqlite3_column_text(stmt, 0);
          if (txt)
            dbPaths.emplace_back(reinterpret_cast<const char *>(txt));
        }
        sqlite3_finalize(stmt);
      }
    }
    for (const auto &p : dbPaths) {
      if (seen.find(p) == seen.end())
        removeMedia(p);
    }
  }

  return true;
}

std::thread LibraryDB::scanDirectoryAsync(const std::string &directory, ProgressCallback progress,
                                          std::atomic<bool> &cancelFlag, bool cleanup) {
  return std::thread([this, directory, progress, &cancelFlag, cleanup]() {
    scanDirectoryImpl(directory, progress, &cancelFlag, cleanup);
  });
}

bool LibraryDB::addMedia(const std::string &path, const std::string &title,
                         const std::string &artist, const std::string &album) {
  if (!m_db)
    return false;
  bool ok = insertMedia(path, title, artist, album, 0, 0, 0, 0);
  if (ok)
    updateSmartPlaylists();
  return ok;
}

bool LibraryDB::updateMedia(const std::string &path, const std::string &title,
                            const std::string &artist, const std::string &album) {
  bool ok = false;
  {
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
    ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
  }
  if (ok)
    updateSmartPlaylists();
  return ok;
}

bool LibraryDB::removeMedia(const std::string &path) {
  bool ok = false;
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_db)
      return false;
    const char *sql = "DELETE FROM MediaItem WHERE path=?1;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
      return false;
    sqlite3_bind_text(stmt, 1, path.c_str(), -1, SQLITE_TRANSIENT);
    ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
  }
  if (ok)
    updateSmartPlaylists();
  return ok;
}

std::vector<MediaMetadata> LibraryDB::search(const std::string &query) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> results;
  if (!m_db)
    return results;
  std::string pattern = "%" + query + "%";
  const char *sql = "SELECT path,title,artist,album,duration,width,height FROM MediaItem "
                    "WHERE title LIKE ?1 COLLATE NOCASE OR artist LIKE ?1 COLLATE NOCASE OR album "
                    "LIKE ?1 COLLATE NOCASE "
                    "ORDER BY title;";
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

std::vector<MediaMetadata> LibraryDB::smartQuery(const std::string &filter) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> results;
  if (!m_db)
    return results;

  auto isIdentChar = [](char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; };
  size_t pos = 0;
  auto skipSpaces = [&]() {
    while (pos < filter.size() && std::isspace(static_cast<unsigned char>(filter[pos])))
      ++pos;
  };
  auto parseIdent = [&]() -> std::string {
    size_t start = pos;
    while (pos < filter.size() && isIdentChar(filter[pos]))
      ++pos;
    return filter.substr(start, pos - start);
  };
  auto parseOp = [&]() -> std::string {
    static const char *ops[] = {"<=", ">=", "!=", "<", ">", "="};
    for (auto op : ops) {
      size_t len = std::strlen(op);
      if (filter.compare(pos, len, op) == 0) {
        pos += len;
        return op;
      }
    }
    return {};
  };

  std::string sql = "SELECT path,title,artist,album,duration,width,height FROM MediaItem WHERE ";
  std::vector<std::string> values;
  std::vector<bool> textField;
  int index = 1;

  skipSpaces();
  while (pos < filter.size()) {
    std::string field = parseIdent();
    if (field.empty())
      return results;

    static const char *allowedFields[] = {"path",  "title",  "artist", "album",      "duration",
                                          "width", "height", "rating", "play_count", "last_played"};
    bool allowed = false;
    for (auto f : allowedFields) {
      if (field == f) {
        allowed = true;
        break;
      }
    }
    if (!allowed)
      return results;

    skipSpaces();
    std::string op = parseOp();
    if (op.empty())
      return results;

    skipSpaces();
    bool quoted = false;
    std::string value;
    if (pos < filter.size() && filter[pos] == '\'') {
      quoted = true;
      ++pos;
      size_t start = pos;
      while (pos < filter.size() && filter[pos] != '\'')
        ++pos;
      value = filter.substr(start, pos - start);
      if (pos < filter.size() && filter[pos] == '\'')
        ++pos;
    } else {
      size_t start = pos;
      while (pos < filter.size() && !std::isspace(static_cast<unsigned char>(filter[pos])))
        ++pos;
      value = filter.substr(start, pos - start);
    }

    sql += field + op + "?" + std::to_string(index);
    values.push_back(value);
    textField.push_back(quoted || field == "title" || field == "artist" || field == "album" ||
                        field == "path");
    ++index;

    skipSpaces();
    if (pos >= filter.size())
      break;
    if (filter.compare(pos, 3, "AND") == 0) {
      sql += " AND ";
      pos += 3;
    } else if (filter.compare(pos, 2, "OR") == 0) {
      sql += " OR ";
      pos += 2;
    } else {
      break;
    }
    skipSpaces();
  }
  sql += " ORDER BY title;";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    return results;
  for (size_t i = 0; i < values.size(); ++i) {
    if (textField[i])
      sqlite3_bind_text(stmt, static_cast<int>(i + 1), values[i].c_str(), -1, SQLITE_TRANSIENT);
    else
      sqlite3_bind_int(stmt, static_cast<int>(i + 1), std::stoi(values[i]));
  }

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

std::vector<MediaMetadata> LibraryDB::allMedia() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> items;
  if (!m_db)
    return items;
  const char *sql =
      "SELECT path,title,artist,album,duration,width,height FROM MediaItem ORDER BY title;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return items;
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

std::vector<std::string> LibraryDB::allPlaylists() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<std::string> names;
  if (!m_db)
    return names;
  const char *sql = "SELECT name FROM Playlist ORDER BY name;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return names;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const unsigned char *txt = sqlite3_column_text(stmt, 0);
    if (txt)
      names.emplace_back(reinterpret_cast<const char *>(txt));
  }
  sqlite3_finalize(stmt);
  return names;
}

std::vector<MediaMetadata> LibraryDB::recentlyAdded(int limit) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> items;
  if (!m_db)
    return items;
  const char *sql = "SELECT path,title,artist,album,duration,width,height FROM MediaItem ORDER BY "
                    "last_played DESC LIMIT ?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return items;
  sqlite3_bind_int(stmt, 1, limit);
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

std::vector<MediaMetadata> LibraryDB::mostPlayed(int limit) {
  std::lock_guard<std::mutex> lock(m_mutex);
  std::vector<MediaMetadata> items;
  if (!m_db)
    return items;
  const char *sql = "SELECT path,title,artist,album,duration,width,height FROM MediaItem ORDER BY "
                    "play_count DESC LIMIT ?1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return items;
  sqlite3_bind_int(stmt, 1, limit);
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

void LibraryDB::setRecommender(AIRecommender *recommender) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_recommender = recommender;
}

std::vector<MediaMetadata> LibraryDB::recommendations() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_recommender)
    return m_recommender->recommend(*this);
  return {};
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

bool LibraryDB::updateSmartPlaylists() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (!m_db)
    return false;
  const char *sql = "SELECT playlist_id, definition FROM SmartPlaylist;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  bool ok = true;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int pid = sqlite3_column_int(stmt, 0);
    const unsigned char *txt = sqlite3_column_text(stmt, 1);
    std::string def;
    if (txt)
      def = reinterpret_cast<const char *>(txt);

    sqlite3_stmt *del = nullptr;
    if (sqlite3_prepare_v2(m_db, "DELETE FROM PlaylistItem WHERE playlist_id=?1;", -1, &del,
                           nullptr) != SQLITE_OK) {
      ok = false;
      break;
    }
    sqlite3_bind_int(del, 1, pid);
    sqlite3_step(del);
    sqlite3_finalize(del);

    std::string q = "SELECT path FROM MediaItem WHERE " + def + " ORDER BY title;";
    sqlite3_stmt *sel = nullptr;
    if (sqlite3_prepare_v2(m_db, q.c_str(), -1, &sel, nullptr) != SQLITE_OK) {
      ok = false;
      break;
    }
    int pos = 0;
    while (sqlite3_step(sel) == SQLITE_ROW) {
      const unsigned char *p = sqlite3_column_text(sel, 0);
      if (!p)
        continue;
      sqlite3_stmt *ins = nullptr;
      if (sqlite3_prepare_v2(
              m_db, "INSERT INTO PlaylistItem (playlist_id, path, position) VALUES (?1, ?2, ?3);",
              -1, &ins, nullptr) != SQLITE_OK) {
        ok = false;
        break;
      }
      sqlite3_bind_int(ins, 1, pid);
      sqlite3_bind_text(ins, 2, reinterpret_cast<const char *>(p), -1, SQLITE_TRANSIENT);
      sqlite3_bind_int(ins, 3, pos++);
      sqlite3_step(ins);
      sqlite3_finalize(ins);
    }
    sqlite3_finalize(sel);
    if (!ok)
      break;
  }
  sqlite3_finalize(stmt);
  return ok;
}

} // namespace mediaplayer
