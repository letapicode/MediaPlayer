#ifndef MEDIAPLAYER_LIBRARYFACADE_H
#define MEDIAPLAYER_LIBRARYFACADE_H

#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/LibraryScanner.h"
#include "mediaplayer/LibraryWorker.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>

namespace mediaplayer {

class LibraryFacade {
public:
  using MediaListCallback = LibraryWorker::MediaListCallback;
  using PlaylistListCallback = LibraryWorker::PlaylistListCallback;
  using ProgressCallback = LibraryDB::ProgressCallback;

  LibraryFacade();
  ~LibraryFacade();

  void setLibrary(LibraryDB *db);

  void startScan(const std::string &directory, ProgressCallback progress = {},
                 std::function<void()> done = {}, bool cleanup = true);
  void scanFile(const std::string &file, std::function<void()> done = {});
  void cancelScan();
  bool scanRunning() const;
  size_t current() const;
  size_t total() const;

  void asyncAllMedia(MediaListCallback cb);
  void asyncAllPlaylists(PlaylistListCallback cb);
  void asyncPlaylistItems(const std::string &name, MediaListCallback cb);
  std::vector<std::string> tags(const std::string &path) const;
  void asyncTags(const std::string &path, LibraryWorker::TagsCallback cb);

private:
  LibraryDB *m_db{nullptr};
  std::unique_ptr<LibraryScanner> m_scanner;
  std::unique_ptr<LibraryWorker> m_worker;
  std::thread m_waitThread;
  std::atomic<bool> m_fileScan{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYFACADE_H
