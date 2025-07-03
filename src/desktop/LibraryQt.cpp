#include "LibraryQt.h"
#include <QVariant>

using namespace mediaplayer;

LibraryQt::LibraryQt(QObject *parent) : QObject(parent) {}

LibraryQt::~LibraryQt() {
  cancelScan();
  if (m_waitThread.joinable())
    m_waitThread.join();
  if (m_worker)
    m_worker->stop();
}

void LibraryQt::setLibrary(LibraryDB *db) {
  m_db = db;
  if (m_worker)
    m_worker->stop();
  if (m_db)
    m_worker = std::make_unique<LibraryWorker>(*m_db);
  else
    m_worker.reset();
}

void LibraryQt::startScan(const QString &directory, bool cleanup) {
  if (!m_db)
    return;
  cancelScan();
  m_scanner = std::make_unique<LibraryScanner>(*m_db);
  m_scanner->start(
      directory.toStdString(),
      [this](size_t c, size_t t) { emit scanProgress(static_cast<int>(c), static_cast<int>(t)); },
      cleanup);
  m_waitThread = std::thread([this]() {
    if (m_scanner)
      m_scanner->wait();
    emit scanFinished();
  });
}

void LibraryQt::scanFile(const QString &file) {
  if (!m_db)
    return;
  cancelScan();
  m_fileScan.store(true);
  m_waitThread = std::thread([this, path = file.toStdString()]() {
    m_db->scanFile(path);
    m_fileScan.store(false);
    emit scanFinished();
  });
}

void LibraryQt::cancelScan() {
  if (m_scanner)
    m_scanner->cancel();
  if (m_waitThread.joinable())
    m_waitThread.join();
  m_scanner.reset();
  m_fileScan.store(false);
}

bool LibraryQt::scanRunning() const {
  if (m_scanner && m_scanner->isRunning())
    return true;
  return m_fileScan.load();
}

int LibraryQt::current() const {
  if (m_scanner)
    return static_cast<int>(m_scanner->current());
  return 0;
}

int LibraryQt::total() const {
  if (m_scanner)
    return static_cast<int>(m_scanner->total());
  return 0;
}

static QVariantMap toMap(const MediaMetadata &m) {
  QVariantMap v;
  v["path"] = QString::fromStdString(m.path);
  v["title"] = QString::fromStdString(m.title);
  v["artist"] = QString::fromStdString(m.artist);
  v["album"] = QString::fromStdString(m.album);
  v["genre"] = QString::fromStdString(m.genre);
  v["duration"] = m.duration;
  v["width"] = m.width;
  v["height"] = m.height;
  v["rating"] = m.rating;
  return v;
}

QList<QVariantMap> LibraryQt::allMedia() const {
  QList<QVariantMap> list;
  if (!m_db)
    return list;
  for (const auto &m : m_db->allMedia())
    list.append(toMap(m));
  return list;
}

QStringList LibraryQt::allPlaylists() const {
  QStringList list;
  if (!m_db)
    return list;
  for (const auto &p : m_db->allPlaylists())
    list.append(QString::fromStdString(p));
  return list;
}

QList<QVariantMap> LibraryQt::playlistItems(const QString &name) const {
  QList<QVariantMap> list;
  if (!m_db)
    return list;
  for (const auto &m : m_db->playlistItems(name.toStdString()))
    list.append(toMap(m));
  return list;
}

void LibraryQt::asyncAllMedia() {
  if (!m_worker)
    return;
  m_worker->asyncAllMedia([this](std::vector<MediaMetadata> media) {
    QList<QVariantMap> list;
    for (const auto &m : media)
      list.append(toMap(m));
    emit mediaListReady(list);
  });
}

void LibraryQt::asyncAllPlaylists() {
  if (!m_worker)
    return;
  m_worker->asyncAllPlaylists([this](std::vector<std::string> names) {
    QStringList list;
    for (const auto &n : names)
      list.append(QString::fromStdString(n));
    emit playlistListReady(list);
  });
}

void LibraryQt::asyncPlaylistItems(const QString &name) {
  if (!m_worker)
    return;
  auto n = name.toStdString();
  m_worker->asyncPlaylistItems(n, [this, name](std::vector<MediaMetadata> items) {
    QList<QVariantMap> list;
    for (const auto &m : items)
      list.append(toMap(m));
    emit playlistItemsReady(name, list);
  });
}
