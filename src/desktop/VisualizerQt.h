#ifndef MEDIAPLAYER_VISUALIZERQT_H
#define MEDIAPLAYER_VISUALIZERQT_H

#include "mediaplayer/ProjectMVisualizer.h"
#include <QObject>
#include <memory>

namespace mediaplayer {

class VisualizerQt : public QObject {
  Q_OBJECT
  Q_PROPERTY(unsigned texture READ texture NOTIFY textureChanged)

public:
  explicit VisualizerQt(QObject *parent = nullptr);

  void setVisualizer(std::shared_ptr<ProjectMVisualizer> vis);

  Q_INVOKABLE void setEnabled(bool enabled);
  Q_INVOKABLE void render();
  Q_INVOKABLE void nextPreset();
  Q_INVOKABLE void previousPreset();

  unsigned texture() const;

signals:
  void textureChanged();

private:
  std::shared_ptr<ProjectMVisualizer> m_vis;
  bool m_enabled{false};
};

void registerVisualizerQtQmlType();

} // namespace mediaplayer

#endif // MEDIAPLAYER_VISUALIZERQT_H
