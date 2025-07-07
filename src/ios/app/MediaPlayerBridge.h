#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const MediaPlayerTrackFinishedNotification;
extern NSString *const MediaPlayerPositionChangedNotification;
extern NSString *const MediaPlayerTrackLoadedNotification;

@interface MediaPlayerBridge : NSObject
- (instancetype)init;
- (BOOL)open:(NSString *)path;
- (void)play;
- (void)pause;
- (void)stop;
- (void)seek:(double)position;
- (NSArray<NSString *> *)listMedia;
- (NSArray<NSDictionary *> *)allMedia;
- (NSDictionary *)currentMetadata;
- (void)nextTrack;
- (void)previousTrack;
- (void)setCallbacks;
- (void)enableShuffle:(BOOL)enabled;
- (BOOL)shuffleEnabled;
@end

NS_ASSUME_NONNULL_END
