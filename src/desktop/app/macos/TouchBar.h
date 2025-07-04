#import <Cocoa/Cocoa.h>

namespace mediaplayer {
class MediaPlayerController;
}

@interface MPTouchBarController : NSObject <NSTouchBarDelegate>
- (instancetype)initWithController:(mediaplayer::MediaPlayerController *)controller;
@end

void setupTouchBar(mediaplayer::MediaPlayerController *controller);
