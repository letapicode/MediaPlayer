#ifndef MEDIAPLAYER_VISUALIZERITEM_H
#define MEDIAPLAYER_VISUALIZERITEM_H

#include "VisualizerQt.h"
#include <QQuickItem>

namespace mediaplayer {

class VisualizerItem : public QQuickItem {
  Q_OBJECT
  Q_PROPERTY(VisualizerQt *visualizer READ visualizer WRITE setVisualizer NOTIFY visualizerChanged)
  Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
  explicit VisualizerItem(QQuickItem *parent = nullptr);

  VisualizerQt *visualizer() const { return m_visualizer; }
  void setVisualizer(VisualizerQt *vis);

  bool running() const { return m_running; }
  void setRunning(bool r);

signals:
  void visualizerChanged();
  void runningChanged();

protected:
  QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;
  void releaseResources() override;

private:
  VisualizerQt *m_visualizer{nullptr};
  bool m_running{true};
  QSGTexture *m_texture{nullptr};
};

void registerVisualizerItemQmlType();

} // namespace mediaplayer

#endif // MEDIAPLAYER_VISUALIZERITEM_H
