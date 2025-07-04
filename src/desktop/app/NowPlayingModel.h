#ifndef MEDIAPLAYER_NOWPLAYINGMODEL_H
#define MEDIAPLAYER_NOWPLAYINGMODEL_H

#include <QAbstractListModel>
#include <string>
#include <vector>

namespace mediaplayer {
class MediaPlayer;

class NowPlayingModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles { PathRole = Qt::UserRole + 1 };
  explicit NowPlayingModel(MediaPlayer *player, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void refresh();
  Q_INVOKABLE void removeAt(int row);
  Q_INVOKABLE void moveItem(int from, int to);

signals:
  void queueChanged();

private:
  MediaPlayer *m_player{nullptr};
  std::vector<std::string> m_items;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NOWPLAYINGMODEL_H
