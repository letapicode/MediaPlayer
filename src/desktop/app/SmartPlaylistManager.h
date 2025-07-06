#ifndef MEDIAPLAYER_SMARTPLAYLISTMANAGER_H
#define MEDIAPLAYER_SMARTPLAYLISTMANAGER_H

#include <QObject>
#include <QVariantList>

namespace mediaplayer {

class PlaylistModel;

class SmartPlaylistManager : public QObject {
  Q_OBJECT
public:
  explicit SmartPlaylistManager(QObject *parent = nullptr);

  void setPlaylistModel(PlaylistModel *model);

  Q_INVOKABLE void createSmartPlaylist(const QString &name, const QVariantList &rules);

private:
  PlaylistModel *m_playlistModel{nullptr};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_SMARTPLAYLISTMANAGER_H
