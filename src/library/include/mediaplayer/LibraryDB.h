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

class LibraryDB {
public:
  explicit LibraryDB(const std::string &path = "media_library.db");
  ~LibraryDB();

  bool open();
  void close();
  bool initSchema();
  bool scanDirectory(const std::string &directory);

  using ProgressCallback = std::function<void(size_t current, size_t total)>;
  // Scan a directory asynchronously. Progress is reported via the callback and
  // scanning can be cancelled by setting cancelFlag to true.
  std::thread scanDirectoryAsync(const std::string &directory, ProgressCallback progress,
                                 std::atomic<bool> &cancelFlag);

  // Insert a media entry directly. Useful for tests or manual additions.
  bool addMedia(const std::string &path, const std::string &title, const std::string &artist,
                const std::string &album);

  // Update metadata for an existing media entry identified by path.
  bool updateMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album);

  // Remove a media item from the database by path.
  bool removeMedia(const std::string &path);

  // Search library for items where title, artist or album contain the query
  // string. Case-insensitive according to SQLite's LIKE operator.
  std::vector<MediaMetadata> search(const std::string &query);

  // Retrieve all media entries in the library ordered by title.
  std::vector<MediaMetadata> allMedia() const;

  // Get the list of playlist names.
  std::vector<std::string> allPlaylists() const;

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

private:
  bool insertMedia(const std::string &path, const std::string &title, const std::string &artist,
                   const std::string &album, int duration = 0, int width = 0, int height = 0,
                   int rating = 0);
  int playlistId(const std::string &name) const;
  bool scanDirectoryImpl(const std::string &directory, ProgressCallback progress,
                         std::atomic<bool> *cancelFlag);

private:
  std::string m_path;
  sqlite3 *m_db{nullptr};
  mutable std::mutex m_mutex;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYDB_H
