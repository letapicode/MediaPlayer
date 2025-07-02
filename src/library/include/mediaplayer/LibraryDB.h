#ifndef MEDIAPLAYER_LIBRARYDB_H
#define MEDIAPLAYER_LIBRARYDB_H

#include "mediaplayer/MediaMetadata.h"
#include <atomic>
#include <functional>
#include <mutex>
#include <sqlite3.h>
#include <string>
#include <thread>
#include <vector>

namespace mediaplayer {

class AIRecommender;

class LibraryDB {
public:
  explicit LibraryDB(const std::string &path = "media_library.db");
  ~LibraryDB();

  bool open();
  void close();
  bool initSchema();
  bool scanDirectory(const std::string &directory, bool cleanup = true);

  using ProgressCallback = std::function<void(size_t current, size_t total)>;
  // Scan a directory asynchronously. Progress is reported via the callback and
  // scanning can be cancelled by setting cancelFlag to true. Callers must join
  // the returned thread before destroying the LibraryDB object.
  std::thread scanDirectoryAsync(const std::string &directory, ProgressCallback progress,
                                 std::atomic<bool> &cancelFlag, bool cleanup = true);

  // Insert a media entry directly. Useful for tests or manual additions.
  bool addMedia(const std::string &path, const std::string &title, const std::string &artist,
                const std::string &album);

  // Update metadata for an existing media entry identified by path.
  bool updateMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album);

  // Remove a media item from the database by path.
  bool removeMedia(const std::string &path);

  // Search library using full text search over title, artist and album.
  // Powered by SQLite FTS5 and case-insensitive.
  std::vector<MediaMetadata> search(const std::string &query);

  // Execute a simple filter expression against MediaItem fields. Supported
  // operators are =, !=, <, >, <= and >= combined with AND/OR. Example:
  //   "rating>=4 AND artist='Queen'".
  std::vector<MediaMetadata> smartQuery(const std::string &filter);

  // Increment play count and update last played timestamp for a media item.
  bool recordPlayback(const std::string &path);

  // Rating management (0-5). Values outside range will be clamped.
  bool setRating(const std::string &path, int rating);
  int rating(const std::string &path) const;

  // Playlist management
  bool createPlaylist(const std::string &name);
  bool deletePlaylist(const std::string &name);
  bool addToPlaylist(const std::string &name, const std::string &path);
  bool removeFromPlaylist(const std::string &name, const std::string &path);
  std::vector<MediaMetadata> playlistItems(const std::string &name);

  // Retrieve full media and playlist lists
  std::vector<MediaMetadata> allMedia() const;
  std::vector<std::string> allPlaylists() const;

  // Convenience queries
  std::vector<MediaMetadata> recentlyAdded(int limit);
  std::vector<MediaMetadata> mostPlayed(int limit);

  // AI recommendation support
  void setRecommender(AIRecommender *recommender);
  std::vector<MediaMetadata> recommendations();

  // Re-evaluate smart playlists against current library contents.
  bool updateSmartPlaylists();

private:
  bool insertMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album, int duration = 0, int width = 0, int height = 0,
                   int rating = 0);
  int playlistId(const std::string &name) const;
  bool scanDirectoryImpl(const std::string &directory, ProgressCallback progress,
                         std::atomic<bool> *cancelFlag, bool cleanup);

private:
  std::string m_path;
  sqlite3 *m_db{nullptr};
  mutable std::mutex m_mutex;
  AIRecommender *m_recommender{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYDB_H
