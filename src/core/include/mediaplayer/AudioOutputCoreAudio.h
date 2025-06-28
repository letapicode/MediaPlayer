#ifndef MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H
#define MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H

#include "AudioOutput.h"
#include "RingBuffer.h"
#include <AudioToolbox/AudioToolbox.h>

namespace mediaplayer {

class AudioOutputCoreAudio : public AudioOutput {
public:
  AudioOutputCoreAudio();
  ~AudioOutputCoreAudio() override;

  bool init(int sampleRate, int channels) override;
  void shutdown() override;
  int write(const uint8_t *data, int len) override;
  void pause() override;
  void resume() override;

private:
  friend void BufferCallback(void *, AudioQueueRef, AudioQueueBufferRef);
  AudioQueueRef m_queue{nullptr};
  AudioStreamBasicDescription m_format{};
  bool m_started{false};
  bool m_paused{false};
  RingBuffer m_buffer{16384};
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_AUDIOOUTPUTCOREAUDIO_H
