#include "mediaplayer/AudioOutputiOS.h"

#import <AVFoundation/AVFoundation.h>

namespace mediaplayer {

AudioOutputiOS::AudioOutputiOS() = default;

AudioOutputiOS::~AudioOutputiOS() { shutdown(); }

bool AudioOutputiOS::init(int sampleRate, int channels) {
  @autoreleasepool {
    m_engine = [[AVAudioEngine alloc] init];
    m_player = [[AVAudioPlayerNode alloc] init];
    [(__bridge AVAudioEngine *)m_engine attachNode:(__bridge AVAudioPlayerNode *)m_player];

    AVAudioFormat *format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:sampleRate
                                                                           channels:channels];
    m_format = (__bridge_retained void *)format;
    [(__bridge AVAudioEngine *)m_engine
        connect:(__bridge AVAudioPlayerNode *)m_player
             to:(__bridge AVAudioNode *)[(AVAudioEngine *)m_engine mainMixerNode]
         format:format];
    NSError *error = nil;
    if (![(__bridge AVAudioEngine *)m_engine startAndReturnError:&error]) {
      shutdown();
      return false;
    }
    [(__bridge AVAudioPlayerNode *)m_player play];
    m_paused = false;
    m_volume = 1.0;
    return true;
  }
}

void AudioOutputiOS::shutdown() {
  @autoreleasepool {
    if (m_player) {
      [(__bridge AVAudioPlayerNode *)m_player stop];
      m_player = nullptr;
    }
    if (m_engine) {
      [(__bridge AVAudioEngine *)m_engine stop];
      m_engine = nullptr;
    }
    if (m_format) {
      CFRelease(m_format);
      m_format = nullptr;
    }
  }
}

int AudioOutputiOS::write(const uint8_t *data, int len) {
  @autoreleasepool {
    if (!m_engine || !m_player || m_paused)
      return 0;
    AVAudioFormat *format = (__bridge AVAudioFormat *)m_format;
    NSUInteger frameSize = format.streamDescription->mBytesPerFrame;
    NSUInteger frames = len / frameSize;
    AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format
                                                             frameCapacity:frames];
    buffer.frameLength = frames;
    memcpy(buffer.int16ChannelData[0], data, len);
    [(__bridge AVAudioPlayerNode *)m_player scheduleBuffer:buffer completionHandler:nil];
    return len;
  }
}

void AudioOutputiOS::pause() {
  if (m_player && !m_paused) {
    [(__bridge AVAudioPlayerNode *)m_player pause];
    m_paused = true;
  }
}

void AudioOutputiOS::resume() {
  if (m_player && m_paused) {
    [(__bridge AVAudioPlayerNode *)m_player play];
    m_paused = false;
  }
}

void AudioOutputiOS::setVolume(double volume) {
  if (volume < 0.0)
    volume = 0.0;
  else if (volume > 1.0)
    volume = 1.0;
  m_volume = volume;
  if (m_player)
    [(__bridge AVAudioPlayerNode *)m_player setVolume:static_cast<float>(volume)];
}

double AudioOutputiOS::volume() const { return m_volume; }

} // namespace mediaplayer
