#include "mediaplayer/LibraryWorker.h"

namespace mediaplayer {

LibraryWorker::LibraryWorker(LibraryDB &db) : m_db(db) {
  m_thread = std::thread(&LibraryWorker::threadLoop, this);
}

LibraryWorker::~LibraryWorker() { stop(); }

void LibraryWorker::stop() {
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stop)
      return;
    m_stop = true;
  }
  m_cv.notify_all();
  if (m_thread.joinable())
    m_thread.join();
}

void LibraryWorker::asyncAllMedia(MediaListCallback cb) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_tasks.push({[this, cb]() {
    if (cb)
      cb(m_db.allMedia());
  }});
  m_cv.notify_one();
}

void LibraryWorker::asyncAllPlaylists(PlaylistListCallback cb) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_tasks.push({[this, cb]() {
    if (cb)
      cb(m_db.allPlaylists());
  }});
  m_cv.notify_one();
}

void LibraryWorker::asyncPlaylistItems(const std::string &name, MediaListCallback cb) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_tasks.push({[this, name, cb]() {
    if (cb)
      cb(m_db.playlistItems(name));
  }});
  m_cv.notify_one();
}

void LibraryWorker::threadLoop() {
  while (true) {
    Task task;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv.wait(lock, [this]() { return m_stop || !m_tasks.empty(); });
      if (m_stop && m_tasks.empty())
        break;
      task = std::move(m_tasks.front());
      m_tasks.pop();
    }
    task.func();
  }
}

} // namespace mediaplayer
