#ifndef MEDIAPLAYER_LIBRARYWORKER_H
#define MEDIAPLAYER_LIBRARYWORKER_H

#include "mediaplayer/LibraryDB.h"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace mediaplayer {

class LibraryWorker {
public:
  explicit LibraryWorker(LibraryDB &db);
  ~LibraryWorker();

  LibraryWorker(const LibraryWorker &) = delete;
  LibraryWorker &operator=(const LibraryWorker &) = delete;

  using MediaListCallback = std::function<void(std::vector<MediaMetadata>)>;
  using PlaylistListCallback = std::function<void(std::vector<std::string>)>;

  // Queue database operations on the worker thread
  void asyncAllMedia(MediaListCallback cb);
  void asyncAllPlaylists(PlaylistListCallback cb);
  void asyncPlaylistItems(const std::string &name, MediaListCallback cb);

  void stop();

private:
  struct Task {
    std::function<void()> func;
  };

  void threadLoop();

  LibraryDB &m_db;
  std::thread m_thread;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  bool m_stop{false};
  std::queue<Task> m_tasks;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYWORKER_H
