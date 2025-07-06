#ifndef MEDIAPLAYER_LIBRARYMODEL_H
#define MEDIAPLAYER_LIBRARYMODEL_H

#include "mediaplayer/LibraryDB.h"
#include <QAbstractListModel>

namespace mediaplayer {

class LibraryModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles {
    PathRole = Qt::UserRole + 1,
    TitleRole,
    ArtistRole,
    AlbumRole,
    GenreRole,
    DurationRole,
    RatingRole
  };

  explicit LibraryModel(QObject *parent = nullptr);
  void setLibrary(LibraryDB *db);
  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;
  Q_INVOKABLE void search(const QString &text);

private:
  LibraryDB *m_db{nullptr};
  std::vector<MediaMetadata> m_items;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYMODEL_H
