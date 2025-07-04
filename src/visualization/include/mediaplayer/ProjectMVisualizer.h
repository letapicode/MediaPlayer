#ifndef MEDIAPLAYER_PROJECTMVISUALIZER_H
#define MEDIAPLAYER_PROJECTMVISUALIZER_H

#include "mediaplayer/Visualizer.h"
#include <memory>
#include <projectM.hpp>
#include <string>

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
    std::string presetPath{};
    std::string titleFont{};
    std::string menuFont{};
    int smoothPresetDuration{5};
    int presetDuration{30};
    float beatSensitivity{0.0f};
    bool shuffle{true};
  };

  explicit ProjectMVisualizer(const Config &cfg = {});
  ~ProjectMVisualizer() override;

  void onAudioPCM(const int16_t *samples, size_t count, int sampleRate, int channels) override;

  void render();
  void nextPreset();
  void previousPreset();
  void loadPreset(const std::string &file);
  unsigned texture() const { return m_texture; }
  int textureSize() const { return m_config.textureSize; }
  void setMeshSize(int x, int y);
  void setFPS(int fps);
  void setPresetPath(const std::string &path);
  void setFonts(const std::string &title, const std::string &menu);

private:
  void init();

  Config m_config;
  std::unique_ptr<projectM> m_pm;
  unsigned m_texture{0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_PROJECTMVISUALIZER_H
