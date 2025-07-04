#include "SettingsManager.h"

using namespace mediaplayer;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent), m_settings("MediaPlayer", "MediaPlayer") {}

QString SettingsManager::theme() const { return m_settings.value("theme").toString(); }

void SettingsManager::setTheme(const QString &theme) {
  m_settings.setValue("theme", theme);
  m_settings.sync();
  emit themeChanged(theme);
}

QString SettingsManager::audioDevice() const { return m_settings.value("audioDevice").toString(); }

void SettingsManager::setAudioDevice(const QString &name) {
  m_settings.setValue("audioDevice", name);
  m_settings.sync();
  emit audioDeviceChanged(name);
}

double SettingsManager::volume() const { return m_settings.value("volume", 1.0).toDouble(); }

void SettingsManager::setVolume(double volume) {
  m_settings.setValue("volume", volume);
  m_settings.sync();
  emit volumeChanged(volume);
}
