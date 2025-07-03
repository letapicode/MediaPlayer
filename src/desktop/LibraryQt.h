#ifndef MEDIAPLAYER_LIBRARYQT_H
#define MEDIAPLAYER_LIBRARYQT_H

#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/LibraryScanner.h"
#include <QObject>
#include <atomic>
#include <memory>
#include <thread>

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

signals:
  void scanProgress(int current, int total);
  void scanFinished();

private:
  LibraryDB *m_db{nullptr};
  std::unique_ptr<LibraryScanner> m_scanner;
  std::thread m_waitThread;
  std::atomic<bool> m_fileScan{false};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_LIBRARYQT_H
