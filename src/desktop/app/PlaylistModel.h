#ifndef MEDIAPLAYER_PLAYLISTMODEL_H
#define MEDIAPLAYER_PLAYLISTMODEL_H

#include "mediaplayer/LibraryDB.h"
#include <QAbstractListModel>

namespace mediaplayer {

class PlaylistModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles { NameRole = Qt::UserRole + 1 };

  explicit PlaylistModel(QObject *parent = nullptr);
  void setLibrary(LibraryDB *db);
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE void createPlaylist(const QString &name);
  Q_INVOKABLE void removePlaylist(const QString &name);

private:
  LibraryDB *m_db{nullptr};
  QStringList m_playlists;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PLAYLISTMODEL_H
