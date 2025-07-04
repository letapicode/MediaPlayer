#include "AudioDevicesModel.h"
#include <QMediaDevices>

using namespace mediaplayer;

AudioDevicesModel::AudioDevicesModel(QObject *parent) : QAbstractListModel(parent) { refresh(); }

int AudioDevicesModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return static_cast<int>(m_devices.size());
}

QVariant AudioDevicesModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() >= static_cast<int>(m_devices.size()))
    return {};
  const auto &dev = m_devices[index.row()];
  if (role == NameRole)
    return dev.description();
  return {};
}

QHash<int, QByteArray> AudioDevicesModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  return roles;
}

void AudioDevicesModel::refresh() {
  m_devices = QMediaDevices::audioOutputs().toVector().toStdVector();
  beginResetModel();
  endResetModel();
}

QAudioDevice AudioDevicesModel::defaultDevice() const {
  return QMediaDevices::defaultAudioOutput();
}

QAudioDevice AudioDevicesModel::deviceAt(int row) const {
  if (row < 0 || row >= static_cast<int>(m_devices.size()))
    return QAudioDevice();
  return m_devices[row];
}
