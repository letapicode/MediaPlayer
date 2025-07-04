#ifndef MEDIAPLAYER_TRANSLATIONMANAGER_H
#define MEDIAPLAYER_TRANSLATIONMANAGER_H

#include <QObject>
#include <QTranslator>

namespace mediaplayer {

class TranslationManager : public QObject {
  Q_OBJECT
public:
  explicit TranslationManager(QObject *parent = nullptr);

  Q_INVOKABLE void switchLanguage(const QString &locale);

private:
  QTranslator m_translator;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_TRANSLATIONMANAGER_H
