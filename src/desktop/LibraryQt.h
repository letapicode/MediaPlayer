#ifndef MEDIAPLAYER_LIBRARYQT_H
#define MEDIAPLAYER_LIBRARYQT_H

#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/LibraryFacade.h"
#include <QObject>

namespace mediaplayer {

class LibraryQt : public QObject {
  Q_OBJECT
public:
  explicit LibraryQt(QObject *parent = nullptr);
  ~LibraryQt();

  void setLibrary(LibraryDB *db);

  Q_INVOKABLE void startScan(const QString &directory, bool cleanup = true);
  Q_INVOKABLE void scanFile(const QString &file);
  Q_INVOKABLE void cancelScan();
  Q_INVOKABLE bool scanRunning() const;
  Q_INVOKABLE int current() const;
  Q_INVOKABLE int total() const;

  Q_INVOKABLE QList<QVariantMap> allMedia() const;
  Q_INVOKABLE QStringList allPlaylists() const;
  Q_INVOKABLE QList<QVariantMap> playlistItems(const QString &name) const;

  Q_INVOKABLE void asyncAllMedia();
  Q_INVOKABLE void asyncAllPlaylists();
  Q_INVOKABLE void asyncPlaylistItems(const QString &name);

signals:
  void scanProgress(int current, int total);
  void scanFinished();
  void mediaListReady(const QList<QVariantMap> &media);
  void playlistListReady(const QStringList &playlists);
  void playlistItemsReady(const QString &name, const QList<QVariantMap> &items);

private:
  LibraryDB *m_db{nullptr};
  LibraryFacade m_facade;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYQT_H
