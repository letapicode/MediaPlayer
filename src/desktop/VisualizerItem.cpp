#include "VisualizerItem.h"
#include <QQuickWindow>
#include <QSGSimpleTextureNode>

using namespace mediaplayer;

VisualizerItem::VisualizerItem(QQuickItem *parent) : QQuickItem(parent) {
  setFlag(ItemHasContents, true);
}

void VisualizerItem::setVisualizer(VisualizerQt *vis) {
  if (m_visualizer == vis)
    return;
  m_visualizer = vis;
  emit visualizerChanged();
  update();
}

void VisualizerItem::setRunning(bool r) {
  if (m_running == r)
    return;
  m_running = r;
  emit runningChanged();
  update();
}

QSGNode *VisualizerItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
  auto *n = static_cast<QSGSimpleTextureNode *>(node);
  if (!n)
    n = new QSGSimpleTextureNode();

  if (m_visualizer && m_running) {
    m_visualizer->render();
    auto texId = m_visualizer->texture();
    if (texId) {
      int size = m_visualizer->textureSize();
      QSGTexture *texture =
          window()->createTextureFromId(texId, QSize(size, size), QQuickWindow::TextureIsExternal);
      n->setTexture(texture);
      n->setRect(boundingRect());
    }
  }
  return n;
}

void VisualizerItem::releaseResources() {}

void mediaplayer::registerVisualizerItemQmlType() {
  qmlRegisterType<VisualizerItem>("MediaPlayer", 1, 0, "VisualizerItem");
}
