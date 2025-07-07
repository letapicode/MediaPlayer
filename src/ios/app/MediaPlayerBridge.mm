#import "MediaPlayerBridge.h"
#include "mediaplayer/MediaPlayer.h"
#include <memory>

using namespace mediaplayer;

NSString *const MediaPlayerTrackFinishedNotification = @"MediaPlayerTrackFinished";
NSString *const MediaPlayerPositionChangedNotification = @"MediaPlayerPositionChanged";
NSString *const MediaPlayerTrackLoadedNotification = @"MediaPlayerTrackLoaded";

@interface MediaPlayerBridge () {
  std::unique_ptr<MediaPlayer> _player;
}
@end

@implementation MediaPlayerBridge

- (instancetype)init {
  if (self = [super init]) {
    _player = std::make_unique<MediaPlayer>();
  }
  return self;
}

- (BOOL)open:(NSString *)path {
  if (!_player)
    _player = std::make_unique<MediaPlayer>();
  return _player->open(path.UTF8String);
}

- (void)play {
  if (_player)
    _player->play();
}
- (void)pause {
  if (_player)
    _player->pause();
}
- (void)stop {
  if (_player)
    _player->stop();
}
- (void)seek:(double)position {
  if (_player)
    _player->seek(position);
}

- (NSArray<NSString *> *)listMedia {
  if (!_player)
    _player = std::make_unique<MediaPlayer>();
  auto items = _player->allMedia();
  NSMutableArray<NSString *> *arr = [NSMutableArray arrayWithCapacity:items.size()];
  for (const auto &m : items) {
    [arr addObject:[NSString stringWithUTF8String:m.path.c_str()]];
  }
  return arr;
}

- (NSArray<NSDictionary *> *)allMedia {
  if (!_player)
    _player = std::make_unique<MediaPlayer>();
  auto items = _player->allMedia();
  NSMutableArray<NSDictionary *> *arr = [NSMutableArray arrayWithCapacity:items.size()];
  for (const auto &m : items) {
    NSDictionary *info = @{
      @"path" : [NSString stringWithUTF8String:m.path.c_str()],
      @"title" : [NSString stringWithUTF8String:m.title.c_str()],
      @"artist" : [NSString stringWithUTF8String:m.artist.c_str()],
      @"album" : [NSString stringWithUTF8String:m.album.c_str()],
      @"duration" : @(m.duration)
    };
    [arr addObject:info];
  }
  return arr;
}

- (NSDictionary *)currentMetadata {
  if (!_player)
    return @{};
  const auto &m = _player->metadata();
  return @{
    @"path" : [NSString stringWithUTF8String:m.path.c_str()],
    @"title" : [NSString stringWithUTF8String:m.title.c_str()],
    @"artist" : [NSString stringWithUTF8String:m.artist.c_str()],
    @"album" : [NSString stringWithUTF8String:m.album.c_str()],
    @"duration" : @(m.duration)
  };
}

- (void)nextTrack {
  if (_player)
    _player->nextTrack();
}
- (void)previousTrack {
  if (_player)
    _player->previousTrack();
}

- (void)setCallbacks {
  if (!_player)
    _player = std::make_unique<MediaPlayer>();
  PlaybackCallbacks cbs;
  cbs.onFinished = []() {
    [[NSNotificationCenter defaultCenter] postNotificationName:MediaPlayerTrackFinishedNotification
                                                        object:nil];
  };
  cbs.onPosition = [](double pos) {
    [[NSNotificationCenter defaultCenter]
        postNotificationName:MediaPlayerPositionChangedNotification
                      object:nil
                    userInfo:@{@"position" : @(pos)}];
  };
  cbs.onTrackLoaded = [](const MediaMetadata &meta) {
    NSDictionary *info = @{
      @"path" : [NSString stringWithUTF8String:meta.path.c_str()],
      @"title" : [NSString stringWithUTF8String:meta.title.c_str()],
      @"artist" : [NSString stringWithUTF8String:meta.artist.c_str()],
      @"album" : [NSString stringWithUTF8String:meta.album.c_str()],
      @"duration" : @(meta.duration)
    };
    [[NSNotificationCenter defaultCenter] postNotificationName:MediaPlayerTrackLoadedNotification
                                                        object:nil
                                                      userInfo:info];
  };
  _player->setCallbacks(cbs);
}

@end
