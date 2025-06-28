#include "mediaplayer/AudioOutputAndroid.h"

#ifdef __ANDROID__
#include <android/api-level.h>
#include <cmath>

namespace mediaplayer {

AudioOutputAndroid::AudioOutputAndroid() = default;

AudioOutputAndroid::~AudioOutputAndroid() { shutdown(); }

bool AudioOutputAndroid::init(int sampleRate, int channels) {
#if __ANDROID_API__ >= 26
  if (initAAudio(sampleRate, channels)) {
    m_useAAudio = true;
    return true;
  }
#endif
  m_useAAudio = false;
  return initOpenSL(sampleRate, channels);
}

void AudioOutputAndroid::shutdown() {
#if __ANDROID_API__ >= 26
  if (m_useAAudio && m_stream) {
    AAudioStream_requestStop(m_stream);
    AAudioStream_close(m_stream);
    m_stream = nullptr;
  }
#endif
  if (!m_useAAudio && m_playerObj) {
    (*m_playerObj)->Destroy(m_playerObj);
    m_playerObj = nullptr;
    m_player = nullptr;
    m_bufferQueue = nullptr;
    m_volumeItf = nullptr;
  }
  if (m_outputMixObj) {
    (*m_outputMixObj)->Destroy(m_outputMixObj);
    m_outputMixObj = nullptr;
  }
  if (m_engineObj) {
    (*m_engineObj)->Destroy(m_engineObj);
    m_engineObj = nullptr;
    m_engine = nullptr;
  }
}

int AudioOutputAndroid::write(const uint8_t *data, int len) {
#if __ANDROID_API__ >= 26
  if (m_useAAudio && m_stream) {
    size_t samples = static_cast<size_t>(len) / sizeof(int16_t);
    const int16_t *input = reinterpret_cast<const int16_t *>(data);
    m_buffer.mix(input, samples);

    int channelCount = AAudioStream_getChannelCount(m_stream);
    int16_t temp[1024];
    while (m_buffer.available() >= 1024) {
      size_t n = m_buffer.read(temp, 1024);
      aaudio_result_t written =
          AAudioStream_write(m_stream, temp, static_cast<int>(n / channelCount), 0);
      if (written < 0)
        break;
    }
    return len;
  }
#endif
  if (m_bufferQueue) {
    size_t samples = static_cast<size_t>(len) / sizeof(int16_t);
    const int16_t *input = reinterpret_cast<const int16_t *>(data);
    m_buffer.mix(input, samples);

    int16_t temp[1024];
    while (m_buffer.available() >= 1024) {
      size_t n = m_buffer.read(temp, 1024);
      (*m_bufferQueue)->Enqueue(m_bufferQueue, temp, static_cast<SLuint32>(n * sizeof(int16_t)));
    }
    return len;
  }
  return 0;
}

void AudioOutputAndroid::pause() {
#if __ANDROID_API__ >= 26
  if (m_useAAudio && m_stream) {
    AAudioStream_requestPause(m_stream);
    return;
  }
#endif
  if (m_player)
    (*m_player)->SetPlayState(m_player, SL_PLAYSTATE_PAUSED);
}

void AudioOutputAndroid::resume() {
#if __ANDROID_API__ >= 26
  if (m_useAAudio && m_stream) {
    AAudioStream_requestStart(m_stream);
    return;
  }
#endif
  if (m_player)
    (*m_player)->SetPlayState(m_player, SL_PLAYSTATE_PLAYING);
}

void AudioOutputAndroid::setVolume(double volume) {
  if (volume < 0.0)
    volume = 0.0;
  if (volume > 1.0)
    volume = 1.0;
  m_volume = volume;
#if __ANDROID_API__ >= 26
  if (m_useAAudio && m_stream) {
    AAudioStream_setVolume(m_stream, static_cast<float>(volume));
    return;
  }
#endif
  if (m_volumeItf) {
    SLmillibel level = static_cast<SLmillibel>(2000.0 * log10(volume));
    (*m_volumeItf)->SetVolumeLevel(m_volumeItf, level);
  }
}

double AudioOutputAndroid::volume() const { return m_volume; }

bool AudioOutputAndroid::initAAudio(int sampleRate, int channels) {
  AAudioStreamBuilder *builder = nullptr;
  if (AAudio_createStreamBuilder(&builder) != AAUDIO_OK)
    return false;
  AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
  AAudioStreamBuilder_setSampleRate(builder, sampleRate);
  AAudioStreamBuilder_setChannelCount(builder, channels);
  AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_I16);
  AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
  if (AAudioStreamBuilder_openStream(builder, &m_stream) != AAUDIO_OK) {
    AAudioStreamBuilder_delete(builder);
    return false;
  }
  AAudioStreamBuilder_delete(builder);
  if (AAudioStream_requestStart(m_stream) != AAUDIO_OK) {
    AAudioStream_close(m_stream);
    m_stream = nullptr;
    return false;
  }
  AAudioStream_setVolume(m_stream, static_cast<float>(m_volume));
  return true;
}

bool AudioOutputAndroid::initOpenSL(int sampleRate, int channels) {
  if (slCreateEngine(&m_engineObj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS)
    return false;
  if ((*m_engineObj)->Realize(m_engineObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
    return false;
  if ((*m_engineObj)->GetInterface(m_engineObj, SL_IID_ENGINE, &m_engine) != SL_RESULT_SUCCESS)
    return false;
  if ((*m_engine)->CreateOutputMix(m_engine, &m_outputMixObj, 0, nullptr, nullptr) !=
      SL_RESULT_SUCCESS)
    return false;
  if ((*m_outputMixObj)->Realize(m_outputMixObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
    return false;

  SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
  SLDataFormat_PCM format = {SL_DATAFORMAT_PCM,
                             static_cast<SLuint32>(channels),
                             static_cast<SLuint32>(sampleRate * 1000),
                             SL_PCMSAMPLEFORMAT_FIXED_16,
                             SL_PCMSAMPLEFORMAT_FIXED_16,
                             channels == 2 ? (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT)
                                           : SL_SPEAKER_FRONT_CENTER,
                             SL_BYTEORDER_LITTLEENDIAN};
  SLDataSource audioSrc = {&loc_bufq, &format};
  SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, m_outputMixObj};
  SLDataSink audioSnk = {&loc_outmix, nullptr};
  const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
  const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
  if ((*m_engine)->CreateAudioPlayer(m_engine, &m_playerObj, &audioSrc, &audioSnk, 2, ids, req) !=
      SL_RESULT_SUCCESS)
    return false;
  if ((*m_playerObj)->Realize(m_playerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
    return false;
  if ((*m_playerObj)->GetInterface(m_playerObj, SL_IID_PLAY, &m_player) != SL_RESULT_SUCCESS)
    return false;
  if ((*m_playerObj)->GetInterface(m_playerObj, SL_IID_BUFFERQUEUE, &m_bufferQueue) !=
      SL_RESULT_SUCCESS)
    return false;
  if ((*m_playerObj)->GetInterface(m_playerObj, SL_IID_VOLUME, &m_volumeItf) != SL_RESULT_SUCCESS)
    return false;
  (*m_player)->SetPlayState(m_player, SL_PLAYSTATE_PLAYING);
  setVolume(m_volume);
  return true;
}

} // namespace mediaplayer
#endif // __ANDROID__
