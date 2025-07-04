#include "VisualizerQt.h"
#include <QtQml/qqml.h>

using namespace mediaplayer;

VisualizerQt::VisualizerQt(QObject *parent) : QObject(parent) {}

void VisualizerQt::setVisualizer(std::shared_ptr<ProjectMVisualizer> vis) {
  m_vis = std::move(vis);
  emit textureChanged();
}

void VisualizerQt::setEnabled(bool enabled) { m_enabled = enabled; }

void VisualizerQt::render() {
  if (m_enabled && m_vis) {
    m_vis->render();
    emit textureChanged();
  }
}

unsigned VisualizerQt::texture() const { return m_vis ? m_vis->texture() : 0; }

int VisualizerQt::textureSize() const { return m_vis ? m_vis->textureSize() : 0; }

void VisualizerQt::nextPreset() {
  if (m_vis)
    m_vis->nextPreset();
}

void VisualizerQt::previousPreset() {
  if (m_vis)
    m_vis->previousPreset();
}

void mediaplayer::registerVisualizerQtQmlType() {
  qmlRegisterType<VisualizerQt>("MediaPlayer", 1, 0, "VisualizerQt");
}
