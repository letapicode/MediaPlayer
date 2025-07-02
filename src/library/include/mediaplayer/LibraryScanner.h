#ifndef MEDIAPLAYER_LIBRARYSCANNER_H
#define MEDIAPLAYER_LIBRARYSCANNER_H

#include "mediaplayer/LibraryDB.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

namespace mediaplayer {

class LibraryScanner {
public:
  using ProgressCallback = LibraryDB::ProgressCallback;

  explicit LibraryScanner(LibraryDB &db);
  ~LibraryScanner();

  LibraryScanner(const LibraryScanner &) = delete;
  LibraryScanner &operator=(const LibraryScanner &) = delete;

  void start(const std::string &directory, ProgressCallback progress = nullptr,
             bool cleanup = true);

  void cancel();
  void wait();
  bool isRunning() const;
  size_t current() const;
  size_t total() const;

private:
  LibraryDB &m_db;
  std::thread m_thread;
  std::atomic<bool> m_cancel{false};
  mutable std::mutex m_mutex;
  size_t m_current{0};
  size_t m_total{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYSCANNER_H
