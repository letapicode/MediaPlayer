#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

extern NSString *const MediaPlayerTrackFinishedNotification;
extern NSString *const MediaPlayerPositionChangedNotification;

@interface MediaPlayerBridge : NSObject
- (instancetype)init;
- (BOOL)open:(NSString *)path;
- (void)play;
- (void)pause;
- (void)stop;
- (void)seek:(double)position;
- (NSArray<NSString *> *)listMedia;
- (void)setCallbacks;
@end

NS_ASSUME_NONNULL_END
