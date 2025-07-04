#include "PlaylistModel.h"
#include "mediaplayer/Playlist.h"

using namespace mediaplayer;

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractListModel(parent) {}

void PlaylistModel::setLibrary(LibraryDB *db) {
  m_db = db;
  if (m_db) {
    m_playlists.clear();
    for (const auto &p : m_db->allPlaylists())
      m_playlists.append(QString::fromStdString(p));
    beginResetModel();
    endResetModel();
  }
}

int PlaylistModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return m_playlists.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= m_playlists.size())
    return {};
  if (role == NameRole)
    return m_playlists[index.row()];
  return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  return roles;
}

void PlaylistModel::createPlaylist(const QString &name) {
  if (!m_db)
    return;
  m_db->createPlaylist(name.toStdString());
  m_playlists.append(name);
  beginInsertRows(QModelIndex(), m_playlists.size() - 1, m_playlists.size() - 1);
  endInsertRows();
}

void PlaylistModel::createSmartPlaylist(const QString &name, const QString &filter) {
  if (!m_db)
    return;
  if (!m_db->createSmartPlaylist(name.toStdString(), filter.toStdString()))
    return;
  m_playlists.append(name);
  beginInsertRows(QModelIndex(), m_playlists.size() - 1, m_playlists.size() - 1);
  endInsertRows();
}

void PlaylistModel::removePlaylist(const QString &name) {
  if (!m_db)
    return;
  m_db->deletePlaylist(name.toStdString());
  int row = m_playlists.indexOf(name);
  if (row >= 0) {
    beginRemoveRows(QModelIndex(), row, row);
    m_playlists.removeAt(row);
    endRemoveRows();
  }
}

QList<QVariantMap> PlaylistModel::playlistItems(const QString &name) const {
  QList<QVariantMap> list;
  if (!m_db)
    return list;
  for (const auto &m : m_db->playlistItems(name.toStdString())) {
    QVariantMap map;
    map["path"] = QString::fromStdString(m.path);
    map["title"] = QString::fromStdString(m.title);
    list.append(map);
  }
  return list;
}

void PlaylistModel::addItem(const QString &name, const QString &path) {
  if (!m_db)
    return;
  m_db->addToPlaylist(name.toStdString(), path.toStdString());
}

void PlaylistModel::moveItem(const QString &name, int from, int to) {
  if (!m_db)
    return;
  auto pl = m_db->loadPlaylist(name.toStdString());
  if (from < 0 || to < 0 || from >= static_cast<int>(pl.size()) ||
      to >= static_cast<int>(pl.size()))
    return;
  auto item = pl.items()[from];
  auto items = pl.items();
  items.erase(items.begin() + from);
  items.insert(items.begin() + to, item);
  Playlist updated(name.toStdString());
  for (const auto &p : items)
    updated.addItem(p);
  m_db->savePlaylist(updated);
}

void PlaylistModel::removeFromPlaylist(const QString &name, const QString &path) {
  if (!m_db)
    return;
  m_db->removeFromPlaylist(name.toStdString(), path.toStdString());
}
