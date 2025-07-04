#ifndef MEDIAPLAYER_AUDIODEVICESMODEL_H
#define MEDIAPLAYER_AUDIODEVICESMODEL_H

#include <QAbstractListModel>
#include <QAudioDevice>
#include <vector>

namespace mediaplayer {

class AudioDevicesModel : public QAbstractListModel {
  Q_OBJECT
public:
  enum Roles { NameRole = Qt::UserRole + 1 };
  explicit AudioDevicesModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void refresh();
  QAudioDevice defaultDevice() const;
  QAudioDevice deviceAt(int row) const;

private:
  std::vector<QAudioDevice> m_devices;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIODEVICESMODEL_H
