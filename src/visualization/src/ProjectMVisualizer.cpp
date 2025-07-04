#include "mediaplayer/ProjectMVisualizer.h"

using namespace mediaplayer;

ProjectMVisualizer::ProjectMVisualizer(const Config &cfg) : m_config(cfg) { init(); }

ProjectMVisualizer::~ProjectMVisualizer() = default;

void ProjectMVisualizer::init() {
  projectM::Settings s{};
  s.meshX = m_config.meshX;
  s.meshY = m_config.meshY;
  s.fps = m_config.fps;
  s.textureSize = m_config.textureSize;
  s.windowWidth = m_config.width;
  s.windowHeight = m_config.height;
  s.presetURL = "";
  s.titleFontURL = "";
  s.menuFontURL = "";
  s.smoothPresetDuration = 5;
  s.presetDuration = 30;
  s.beatSensitivity = 0.0f;
  s.aspectCorrection = true;
  s.easterEgg = 0.0f;
  s.shuffleEnabled = true;
  s.softCutRatingsEnabled = false;
  m_pm = std::make_unique<projectM>(s);
  m_texture = m_pm->initRenderToTexture();
}

void ProjectMVisualizer::onAudioPCM(const int16_t *samples, size_t count, int /*sampleRate*/,
                                    int /*channels*/) {
  if (!m_pm || !samples || count == 0)
    return;
  if (count > 0)
    m_pm->pcm()->addPCM16Data(samples, static_cast<short>(count));
}

void ProjectMVisualizer::render() {
  if (m_pm)
    m_pm->renderFrame();
}

void ProjectMVisualizer::nextPreset() {
  if (m_pm)
    m_pm->selectNext(true);
}

void ProjectMVisualizer::previousPreset() {
  if (m_pm)
    m_pm->selectPrevious(true);
}

void ProjectMVisualizer::setMeshSize(int x, int y) {
  if (m_config.meshX == x && m_config.meshY == y)
    return;
  m_config.meshX = x;
  m_config.meshY = y;
  init();
}

void ProjectMVisualizer::setFPS(int fps) {
  if (m_config.fps == fps)
    return;
  m_config.fps = fps;
  init();
}
