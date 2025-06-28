#include "mediaplayer/AudioOutputCoreAudio.h"

#include <cstring>

namespace mediaplayer {

static void BufferCallback(void *userData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
  AudioQueueFreeBuffer(inAQ, inBuffer);
}

AudioOutputCoreAudio::AudioOutputCoreAudio() = default;

AudioOutputCoreAudio::~AudioOutputCoreAudio() { shutdown(); }

bool AudioOutputCoreAudio::init(int sampleRate, int channels) {
  m_format.mSampleRate = sampleRate;
  m_format.mFormatID = kAudioFormatLinearPCM;
  m_format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
  m_format.mBitsPerChannel = 16;
  m_format.mChannelsPerFrame = static_cast<UInt32>(channels);
  m_format.mBytesPerFrame = m_format.mChannelsPerFrame * m_format.mBitsPerChannel / 8;
  m_format.mFramesPerPacket = 1;
  m_format.mBytesPerPacket = m_format.mBytesPerFrame;
  m_format.mReserved = 0;

  OSStatus status =
      AudioQueueNewOutput(&m_format, BufferCallback, this, nullptr, nullptr, 0, &m_queue);
  if (status != noErr)
    return false;
  status = AudioQueueStart(m_queue, nullptr);
  if (status != noErr) {
    AudioQueueDispose(m_queue, true);
    m_queue = nullptr;
    return false;
  }
  m_started = true;
  return true;
}

void AudioOutputCoreAudio::shutdown() {
  if (m_queue) {
    AudioQueueStop(m_queue, true);
    AudioQueueDispose(m_queue, true);
    m_queue = nullptr;
  }
  m_started = false;
}

int AudioOutputCoreAudio::write(const uint8_t *data, int len) {
  if (!m_queue || m_paused)
    return 0;
  AudioQueueBufferRef buffer;
  if (AudioQueueAllocateBuffer(m_queue, len, &buffer) != noErr)
    return -1;
  std::memcpy(buffer->mAudioData, data, len);
  buffer->mAudioDataByteSize = static_cast<UInt32>(len);
  if (AudioQueueEnqueueBuffer(m_queue, buffer, 0, nullptr) != noErr) {
    AudioQueueFreeBuffer(m_queue, buffer);
    return -1;
  }
  return len;
}

void AudioOutputCoreAudio::pause() {
  if (m_queue && !m_paused) {
    AudioQueuePause(m_queue);
    m_paused = true;
  }
}

void AudioOutputCoreAudio::resume() {
  if (m_queue && m_paused) {
    AudioQueueStart(m_queue, nullptr);
    m_paused = false;
  }
}

void AudioOutputCoreAudio::setVolume(double volume) { m_volume = volume; }

double AudioOutputCoreAudio::volume() const { return m_volume; }

} // namespace mediaplayer
