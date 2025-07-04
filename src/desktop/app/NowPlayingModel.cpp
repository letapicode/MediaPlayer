#include "NowPlayingModel.h"
#include "mediaplayer/MediaPlayer.h"

using namespace mediaplayer;

NowPlayingModel::NowPlayingModel(MediaPlayer *player, QObject *parent)
    : QAbstractListModel(parent), m_player(player) {
  refresh();
}

int NowPlayingModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_items.size());
}

QVariant NowPlayingModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= static_cast<int>(m_items.size()))
    return {};
  if (role == PathRole)
    return QString::fromStdString(m_items[index.row()]);
  return {};
}

QHash<int, QByteArray> NowPlayingModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[PathRole] = "path";
  return roles;
}

void NowPlayingModel::refresh() {
  if (!m_player)
    return;
  beginResetModel();
  m_items = m_player->queue();
  endResetModel();
  emit queueChanged();
}

void NowPlayingModel::removeAt(int row) {
  if (!m_player || row < 0 || row >= static_cast<int>(m_items.size()))
    return;
  if (m_player->removeFromQueue(static_cast<size_t>(row))) {
    refresh();
  }
}

void NowPlayingModel::moveItem(int from, int to) {
  if (!m_player)
    return;
  if (m_player->moveQueueItem(static_cast<size_t>(from), static_cast<size_t>(to)))
    refresh();
}
