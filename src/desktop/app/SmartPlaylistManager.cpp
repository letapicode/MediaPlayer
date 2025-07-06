#include "SmartPlaylistManager.h"
#include "PlaylistModel.h"

using namespace mediaplayer;

SmartPlaylistManager::SmartPlaylistManager(QObject *parent) : QObject(parent) {}

void SmartPlaylistManager::setPlaylistModel(PlaylistModel *model) { m_playlistModel = model; }

void SmartPlaylistManager::createSmartPlaylist(const QString &name, const QVariantList &rules) {
  if (!m_playlistModel)
    return;
  QString query;
  for (int i = 0; i < rules.size(); ++i) {
    QVariantMap map = rules[i].toMap();
    if (i > 0)
      query += " AND ";
    query += map.value("field").toString() + " " + map.value("op").toString() + " " +
             map.value("value").toString();
  }
  m_playlistModel->createSmartPlaylist(name, query);
}
