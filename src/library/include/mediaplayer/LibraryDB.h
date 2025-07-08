#ifndef MEDIAPLAYER_LIBRARYDB_H
#define MEDIAPLAYER_LIBRARYDB_H

#include "mediaplayer/MediaMetadata.h"
#include "mediaplayer/Playlist.h"
#include <atomic>
#include <condition_variable>
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

  // Scan a single file and insert or update its metadata. Returns true on
  // success. This is useful when adding an individual track from the UI.
  bool scanFile(const std::string &path);

  using ProgressCallback = std::function<void(size_t current, size_t total)>;
  // Scan a directory asynchronously. Progress is reported via the callback and
  // scanning can be cancelled by setting cancelFlag to true. Callers must join
  // the returned thread before destroying the LibraryDB object.
  std::thread scanDirectoryAsync(const std::string &directory, ProgressCallback progress,
                                 std::atomic<bool> &cancelFlag, bool cleanup = true);

  // Asynchronous wrapper for scanFile. The returned thread runs scanFile and
  // terminates when the operation completes.
  std::thread scanFileAsync(const std::string &path);

  // Check if a media item with the given path already exists in the database.
  bool mediaExists(const std::string &path) const;

  // Insert a media entry directly. Useful for tests or manual additions.
  bool addMedia(const std::string &path, const std::string &title, const std::string &artist,
                const std::string &album, const std::string &genre = "");

  // Update metadata for an existing media entry identified by path.
  bool updateMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album, const std::string &genre = "");

  // Remove a media item from the database by path.
  bool removeMedia(const std::string &path);

  // Search library using full text search over title, artist and album.
  // Powered by SQLite FTS5 and case-insensitive.
  std::vector<MediaMetadata> search(const std::string &query);

  // Search using the FTS5 virtual table for more advanced matching. The query
  // should be in FTS syntax.
  std::vector<MediaMetadata> searchFts(const std::string &query);

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
  std::vector<MediaMetadata> playlistItems(const std::string &name) const;
  Playlist loadPlaylist(const std::string &name) const;
  bool savePlaylist(const Playlist &playlist);

  // Smart playlists
  bool createSmartPlaylist(const std::string &name, const std::string &filter);
  bool updateSmartPlaylist(const std::string &name, const std::string &filter);
  bool deleteSmartPlaylist(const std::string &name);

  // Retrieve full media and playlist lists
  std::vector<MediaMetadata> allMedia() const;
  std::vector<std::string> allPlaylists() const;

  // Convenience queries
  std::vector<MediaMetadata> recentlyAdded(int limit);
  std::vector<MediaMetadata> mostPlayed(int limit);

  bool addTags(const std::string &path, const std::vector<std::string> &tags);
  std::vector<std::string> getTags(const std::string &path) const;

  // AI recommendation support
  void setRecommender(AIRecommender *recommender);
  std::vector<MediaMetadata> recommendations();

  // Re-evaluate smart playlists against current library contents.
  bool updateSmartPlaylists();

private:
  void scheduleSmartPlaylistUpdate();

private:
  bool insertMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album, const std::string &genre, int duration = 0,
                   int width = 0, int height = 0, int rating = 0);
  // Helper returning the ID of a playlist. The caller must hold m_mutex
  // before invoking this function.
  int playlistId(const std::string &name) const;
  bool scanDirectoryImpl(const std::string &directory, ProgressCallback progress,
                         std::atomic<bool> *cancelFlag, bool cleanup);

  bool parseSmartFilter(const std::string &filter, std::string &sql,
                        std::vector<std::string> &values, std::vector<bool> &textField) const;

private:
  std::string m_path;
  sqlite3 *m_db{nullptr};
  // Mutex protecting m_db. Most methods lock it internally. Functions marked
  // as requiring the caller to lock (e.g. playlistId) expect it to be held.
  mutable std::mutex m_mutex;
  AIRecommender *m_recommender{nullptr};
  std::thread m_workerThread;
  std::mutex m_workerMutex;
  std::condition_variable m_workerCv;
  bool m_workerStop{false};
  bool m_updateScheduled{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYDB_H
