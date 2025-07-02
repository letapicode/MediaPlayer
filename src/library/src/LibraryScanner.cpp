#include "mediaplayer/LibraryScanner.h"
#include <chrono>

namespace mediaplayer {

LibraryScanner::LibraryScanner(LibraryDB &db) : m_db(db) {}

LibraryScanner::~LibraryScanner() {
  cancel();
  if (m_thread.joinable())
    m_thread.join();
}

void LibraryScanner::start(const std::string &directory, ProgressCallback progress, bool cleanup) {
  cancel();
  if (m_thread.joinable())
    m_thread.join();

  m_cancel.store(false);
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_current = 0;
    m_total = 0;
  }

  auto wrapper = [this, progress](size_t curr, size_t tot) {
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_current = curr;
      m_total = tot;
    }
    if (progress)
      progress(curr, tot);
  };

  m_thread = m_db.scanDirectoryAsync(directory, wrapper, m_cancel, cleanup);
}

void LibraryScanner::cancel() { m_cancel.store(true); }

void LibraryScanner::wait() {
  if (m_thread.joinable())
    m_thread.join();
}

bool LibraryScanner::isRunning() const { return m_thread.joinable(); }

size_t LibraryScanner::current() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_current;
}

size_t LibraryScanner::total() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_total;
}

} // namespace mediaplayer
