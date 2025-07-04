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
  }
  return {};
}

QHash<int, QByteArray> LibraryModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[PathRole] = "path";
  roles[TitleRole] = "title";
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
