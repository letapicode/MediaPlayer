#include "TranslationManager.h"
#include <QGuiApplication>

using namespace mediaplayer;

TranslationManager::TranslationManager(QObject *parent) : QObject(parent) {}

void TranslationManager::switchLanguage(const QString &locale) {
  QGuiApplication::instance()->removeTranslator(&m_translator);
  if (!locale.isEmpty()) {
    m_translator.load("player_" + locale, "translations");
    QGuiApplication::instance()->installTranslator(&m_translator);
  }
}
