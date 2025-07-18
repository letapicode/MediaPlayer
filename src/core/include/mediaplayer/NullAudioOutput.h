#ifndef MEDIAPLAYER_NULLAUDIOOUTPUT_H
#define MEDIAPLAYER_NULLAUDIOOUTPUT_H

#include "AudioOutput.h"
#include <iostream>

namespace mediaplayer {

class NullAudioOutput : public AudioOutput {
public:
  bool init(int sampleRate, int channels) override {
    std::cout << "NullAudioOutput init\n";
    return true;
  }
  void shutdown() override { std::cout << "NullAudioOutput shutdown\n"; }
  int write(const uint8_t *, int len) override {
    return len; // discard
  }
  void pause() override {}
  void resume() override {}
  void setVolume(double volume) override { m_volume = volume; }
  double volume() const override { return m_volume; }

private:
  double m_volume{1.0};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_NULLAUDIOOUTPUT_H
