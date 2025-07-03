#include "mediaplayer/LibraryFacade.h"

#include <utility>

namespace mediaplayer {

LibraryFacade::LibraryFacade() = default;

LibraryFacade::~LibraryFacade() {
  cancelScan();
  if (m_worker)
    m_worker->stop();
}

void LibraryFacade::setLibrary(LibraryDB *db) {
  m_db = db;
  if (m_worker)
    m_worker->stop();
  if (m_db)
    m_worker = std::make_unique<LibraryWorker>(*m_db);
  else
    m_worker.reset();
}

void LibraryFacade::startScan(const std::string &directory, ProgressCallback progress,
                              std::function<void()> done, bool cleanup) {
  if (!m_db)
    return;
  cancelScan();
  m_scanner = std::make_unique<LibraryScanner>(*m_db);
  m_scanner->start(directory, progress, cleanup);
  if (m_worker) {
    m_worker->post([this, done = std::move(done)]() {
      if (m_scanner)
        m_scanner->wait();
      if (done)
        done();
    });
  } else {
    m_waitThread = std::thread([this, done = std::move(done)]() {
      if (m_scanner)
        m_scanner->wait();
      if (done)
        done();
    });
  }
}

void LibraryFacade::scanFile(const std::string &file, std::function<void()> done) {
  if (!m_db)
    return;
  cancelScan();
  m_fileScan.store(true);
  if (m_worker) {
    auto path = file;
    m_worker->post([this, path, done = std::move(done)]() {
      m_db->scanFile(path);
      m_fileScan.store(false);
      if (done)
        done();
    });
  } else {
    m_waitThread = std::thread([this, path = file, done = std::move(done)]() {
      m_db->scanFile(path);
      m_fileScan.store(false);
      if (done)
        done();
    });
  }
}

void LibraryFacade::cancelScan() {
  if (m_scanner)
    m_scanner->cancel();
  if (m_waitThread.joinable())
    m_waitThread.join();
  m_scanner.reset();
  m_fileScan.store(false);
}

bool LibraryFacade::scanRunning() const {
  if (m_scanner && m_scanner->isRunning())
    return true;
  return m_fileScan.load();
}

size_t LibraryFacade::current() const {
  if (m_scanner)
    return m_scanner->current();
  return 0;
}

size_t LibraryFacade::total() const {
  if (m_scanner)
    return m_scanner->total();
  return 0;
}

void LibraryFacade::asyncAllMedia(MediaListCallback cb) {
  if (m_worker)
    m_worker->asyncAllMedia(std::move(cb));
}

void LibraryFacade::asyncAllPlaylists(PlaylistListCallback cb) {
  if (m_worker)
    m_worker->asyncAllPlaylists(std::move(cb));
}

void LibraryFacade::asyncPlaylistItems(const std::string &name, MediaListCallback cb) {
  if (m_worker)
    m_worker->asyncPlaylistItems(name, std::move(cb));
}

} // namespace mediaplayer
