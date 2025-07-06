#include "LibraryModel.h"

using namespace mediaplayer;

LibraryModel::LibraryModel(QObject *parent) : QAbstractListModel(parent) {}

void LibraryModel::setLibrary(LibraryDB *db) {
  m_db = db;
  if (m_db) {
    m_items = m_db->allMedia();
    beginResetModel();
    endResetModel();
  }
}

int LibraryModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return static_cast<int>(m_items.size());
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= static_cast<int>(m_items.size()))
    return {};
  const auto &m = m_items[index.row()];
  switch (role) {
  case PathRole:
    return QString::fromStdString(m.path);
  case TitleRole:
    return QString::fromStdString(m.title);
  case ArtistRole:
    return QString::fromStdString(m.artist);
  case AlbumRole:
    return QString::fromStdString(m.album);
  case GenreRole:
    return QString::fromStdString(m.genre);
  case DurationRole:
    return m.duration;
  case RatingRole:
    return m.rating;
  }
  return {};
}

QHash<int, QByteArray> LibraryModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[PathRole] = "path";
  roles[TitleRole] = "title";
  roles[ArtistRole] = "artist";
  roles[AlbumRole] = "album";
  roles[GenreRole] = "genre";
  roles[DurationRole] = "duration";
  roles[RatingRole] = "rating";
  return roles;
}

void LibraryModel::search(const QString &text) {
  if (!m_db)
    return;
  auto results = m_db->search(text.toStdString());
  beginResetModel();
  m_items = std::move(results);
  endResetModel();
}
