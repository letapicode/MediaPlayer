#ifndef MEDIAPLAYER_PROJECTMVISUALIZER_H
#define MEDIAPLAYER_PROJECTMVISUALIZER_H

#include "mediaplayer/Visualizer.h"
#include <memory>
#include <projectM.hpp>

namespace mediaplayer {

class ProjectMVisualizer : public Visualizer {
public:
  struct Config {
    int width{800};
    int height{600};
    int meshX{32};
    int meshY{24};
    int fps{60};
    int textureSize{512};
  };

  explicit ProjectMVisualizer(const Config &cfg = {});
  ~ProjectMVisualizer() override;

  void onAudioPCM(const int16_t *samples, size_t count, int sampleRate, int channels) override;

  void setGLContext(void *ctx);
  void render();
  void nextPreset();
  void previousPreset();
  unsigned texture() const { return m_texture; }
  int textureSize() const { return m_config.textureSize; }
  void setMeshSize(int x, int y);
  void setFPS(int fps);

private:
  void init();

  Config m_config;
  std::unique_ptr<projectM> m_pm;
  void *m_glContext{nullptr};
  unsigned m_texture{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PROJECTMVISUALIZER_H
