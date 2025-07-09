#import "MediaPlayerBridge.h"
#include "mediaplayer/LibraryDB.h"
#include "mediaplayer/MediaPlayer.h"
#include "mediaplayer/remote/RemoteControlClient.h"
#include <filesystem>
#include <memory>
#include <string>

using namespace mediaplayer;

NSString *const MediaPlayerTrackFinishedNotification = @"MediaPlayerTrackFinished";
NSString *const MediaPlayerPositionChangedNotification = @"MediaPlayerPositionChanged";
NSString *const MediaPlayerTrackLoadedNotification = @"MediaPlayerTrackLoaded";

@interface MediaPlayerBridge () {
  std::unique_ptr<MediaPlayer> _player;
  std::unique_ptr<LibraryDB> _library;
  mediaplayer::remote::RemoteControlClient _rcClient;
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

- (NSArray<NSDictionary *> *)search:(NSString *)query {
  if (!_player)
    _player = std::make_unique<MediaPlayer>();
  auto items = _player->searchLibrary(query.UTF8String);
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
  std::filesystem::path art(m.path);
  art.replace_extension("jpg");
  NSString *artPath = nil;
  if (std::filesystem::exists(art)) {
    std::string s = art.string();
    artPath = [NSString stringWithUTF8String:s.c_str()];
  }
  return @{
    @"path" : [NSString stringWithUTF8String:m.path.c_str()],
    @"title" : [NSString stringWithUTF8String:m.title.c_str()],
    @"artist" : [NSString stringWithUTF8String:m.artist.c_str()],
    @"album" : [NSString stringWithUTF8String:m.album.c_str()],
    @"duration" : @(m.duration),
    @"artwork" : artPath ?: [NSNull null]
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

- (NSString *)currentArtworkPath {
  if (!_player)
    return nil;
  const auto &meta = _player->metadata();
  std::filesystem::path p(meta.path);
  p.replace_extension("jpg");
  if (std::filesystem::exists(p)) {
    std::string s = p.string();
    return [NSString stringWithUTF8String:s.c_str()];
  }
  return nil;
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
    std::filesystem::path art(meta.path);
    art.replace_extension("jpg");
    NSString *artPath = nil;
    if (std::filesystem::exists(art)) {
      std::string s = art.string();
      artPath = [NSString stringWithUTF8String:s.c_str()];
    }
    NSDictionary *info = @{
      @"path" : [NSString stringWithUTF8String:meta.path.c_str()],
      @"title" : [NSString stringWithUTF8String:meta.title.c_str()],
      @"artist" : [NSString stringWithUTF8String:meta.artist.c_str()],
      @"album" : [NSString stringWithUTF8String:meta.album.c_str()],
      @"duration" : @(meta.duration),
      @"artwork" : artPath ?: [NSNull null]
    };
    [[NSNotificationCenter defaultCenter] postNotificationName:MediaPlayerTrackLoadedNotification
                                                        object:nil
                                                      userInfo:info];
  };
  _player->setCallbacks(cbs);
}

- (void)setLibraryPath:(NSString *)path {
  _library = std::make_unique<LibraryDB>(path.UTF8String);
  if (_library->open()) {
    if (!_player)
      _player = std::make_unique<MediaPlayer>();
    _player->setLibrary(_library.get());
  }
}

- (void)enableShuffle:(BOOL)enabled {
  if (_player)
    _player->enableShuffle(enabled);
}

- (BOOL)shuffleEnabled {
  if (_player)
    return _player->shuffleEnabled();
  return NO;
}

- (void)sendSyncTo:(NSString *)address
              port:(uint16_t)port
              path:(NSString *)path
          position:(double)position {
  _rcClient.sendPlay(address.UTF8String, port, path.UTF8String, position);
}

@end
