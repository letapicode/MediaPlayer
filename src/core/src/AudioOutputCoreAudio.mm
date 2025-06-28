#include "mediaplayer/AudioOutputCoreAudio.h"

#include <cstring>

namespace mediaplayer {

static void BufferCallback(void *userData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
  auto *self = static_cast<AudioOutputCoreAudio *>(userData);
  size_t samples = self->m_buffer.read(reinterpret_cast<int16_t *>(inBuffer->mAudioData),
                                       inBuffer->mAudioDataBytesCapacity / sizeof(int16_t));
  inBuffer->mAudioDataByteSize = static_cast<UInt32>(samples * sizeof(int16_t));
  AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, nullptr);
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

  const UInt32 bufferSize = 4096;
  for (int i = 0; i < 4; ++i) {
    AudioQueueBufferRef buf;
    if (AudioQueueAllocateBuffer(m_queue, bufferSize, &buf) != noErr)
      return false;
    std::memset(buf->mAudioData, 0, bufferSize);
    buf->mAudioDataByteSize = bufferSize;
    AudioQueueEnqueueBuffer(m_queue, buf, 0, nullptr);
  }

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

  size_t samples = static_cast<size_t>(len) / sizeof(int16_t);
  const int16_t *input = reinterpret_cast<const int16_t *>(data);
  m_buffer.mix(input, samples);
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

} // namespace mediaplayer
