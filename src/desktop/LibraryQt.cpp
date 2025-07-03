#include "LibraryQt.h"
#include <QVariant>

using namespace mediaplayer;

LibraryQt::LibraryQt(QObject *parent) : QObject(parent) {}

LibraryQt::~LibraryQt() = default;

void LibraryQt::setLibrary(LibraryDB *db) {
  m_db = db;
  m_facade.setLibrary(db);
}

void LibraryQt::startScan(const QString &directory, bool cleanup) {
  if (!m_db)
    return;
  m_facade.startScan(
      directory.toStdString(),
      [this](size_t c, size_t t) { emit scanProgress(static_cast<int>(c), static_cast<int>(t)); },
      [this]() { emit scanFinished(); }, cleanup);
}

void LibraryQt::scanFile(const QString &file) {
  if (!m_db)
    return;
  m_facade.scanFile(file.toStdString(), [this]() { emit scanFinished(); });
}

void LibraryQt::cancelScan() { m_facade.cancelScan(); }

bool LibraryQt::scanRunning() const { return m_facade.scanRunning(); }

int LibraryQt::current() const { return static_cast<int>(m_facade.current()); }

int LibraryQt::total() const { return static_cast<int>(m_facade.total()); }

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
  m_facade.asyncAllMedia([this](std::vector<MediaMetadata> media) {
    QList<QVariantMap> list;
    for (const auto &m : media)
      list.append(toMap(m));
    emit mediaListReady(list);
  });
}

void LibraryQt::asyncAllPlaylists() {
  m_facade.asyncAllPlaylists([this](std::vector<std::string> names) {
    QStringList list;
    for (const auto &n : names)
      list.append(QString::fromStdString(n));
    emit playlistListReady(list);
  });
}

void LibraryQt::asyncPlaylistItems(const QString &name) {
  auto n = name.toStdString();
  m_facade.asyncPlaylistItems(n, [this, name](std::vector<MediaMetadata> items) {
    QList<QVariantMap> list;
    for (const auto &m : items)
      list.append(toMap(m));
    emit playlistItemsReady(name, list);
  });
}
