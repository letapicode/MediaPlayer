#import "TouchBar.h"
#import "../MediaPlayerController.h"

@implementation MPTouchBarController {
  mediaplayer::MediaPlayerController *m_controller;
  NSButton *m_playBtn;
  NSSlider *m_volumeSlider;
}

- (instancetype)initWithController:(mediaplayer::MediaPlayerController *)controller {
  self = [super init];
  if (self) {
    m_controller = controller;
  }
  return self;
}

- (NSTouchBar *)makeTouchBar {
  NSTouchBar *bar = [[NSTouchBar alloc] init];
  bar.delegate = self;
  bar.defaultItemIdentifiers = @[ "play", "prev", "next", "volume" ];
  return bar;
}

- (NSTouchBarItem *)touchBar:(NSTouchBar *)bar
       makeItemForIdentifier:(NSTouchBarItemIdentifier)identifier {
  if ([identifier isEqualToString:@"play"]) {
    m_playBtn = [NSButton buttonWithTitle:@"Play" target:self action:@selector(play)];
    NSCustomTouchBarItem *item = [[NSCustomTouchBarItem alloc] initWithIdentifier:identifier];
    item.view = m_playBtn;
    return item;
  } else if ([identifier isEqualToString:@"prev"]) {
    NSButton *btn = [NSButton buttonWithTitle:@"Prev" target:self action:@selector(prev)];
    NSCustomTouchBarItem *item = [[NSCustomTouchBarItem alloc] initWithIdentifier:identifier];
    item.view = btn;
    return item;
  } else if ([identifier isEqualToString:@"next"]) {
    NSButton *btn = [NSButton buttonWithTitle:@"Next" target:self action:@selector(next)];
    NSCustomTouchBarItem *item = [[NSCustomTouchBarItem alloc] initWithIdentifier:identifier];
    item.view = btn;
    return item;
  } else if ([identifier isEqualToString:@"volume"]) {
    m_volumeSlider = [NSSlider sliderWithValue:1.0
                                      minValue:0
                                      maxValue:1
                                        target:self
                                        action:@selector(volumeChanged:)];
    NSCustomTouchBarItem *item = [[NSCustomTouchBarItem alloc] initWithIdentifier:identifier];
    item.view = m_volumeSlider;
    return item;
  }
  return nil;
}

- (void)play {
  if (m_controller->playing())
    m_controller->pause();
  else
    m_controller->play();
}
- (void)next {
  m_controller->seek(m_controller->position() + 10);
}
- (void)prev {
  m_controller->seek(m_controller->position() - 10);
}
- (void)volumeChanged:(NSSlider *)s {
  m_controller->setVolume(s.doubleValue);
}

- (void)updatePlayButton {
  if (!m_playBtn)
    return;
  NSString *title = m_controller->playing() ? @"Pause" : @"Play";
  [m_playBtn setTitle:title];
}

- (void)updateVolume {
  if (m_volumeSlider)
    m_volumeSlider.doubleValue = m_controller->volume();
}
@end

static MPTouchBarController *g_tb = nil;

void setupTouchBar(mediaplayer::MediaPlayerController *controller) {
  g_tb = [[MPTouchBarController alloc] initWithController:controller];
  [NSApp mainWindow].touchBar = [g_tb makeTouchBar];
  [g_tb updatePlayButton];
  [g_tb updateVolume];
  QObject::connect(controller, &mediaplayer::MediaPlayerController::playbackStateChanged, []() {
    if (g_tb)
      [g_tb updatePlayButton];
  });
  QObject::connect(controller, &mediaplayer::MediaPlayerController::volumeChanged, []() {
    if (g_tb)
      [g_tb updateVolume];
  });
}

void cleanupTouchBar() {
  if (!g_tb)
    return;
  [NSApp mainWindow].touchBar = nil;
  g_tb = nil;
}
