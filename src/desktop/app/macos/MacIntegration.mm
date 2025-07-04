#import <Cocoa/Cocoa.h>
#import <MediaPlayer/MediaPlayer.h>

#include <QGuiApplication>
#include <QKeyEvent>

#include "mediaplayer/MediaMetadata.h"

using namespace mediaplayer;

static CFMachPortRef sEventTap = nullptr;

static CGEventRef mediaKeyCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event,
                                   void *refcon) {
  if (type != NX_SYSDEFINED)
    return event;
  NSEvent *nsEvent = [NSEvent eventWithCGEvent:event];
  if ([nsEvent subtype] != NX_SUBTYPE_AUX_CONTROL_BUTTONS)
    return event;

  int keyCode = ([nsEvent data1] & 0xFFFF0000) >> 16;
  int keyFlags = ([nsEvent data1] & 0x0000FFFF);
  bool keyDown = ((keyFlags & 0xFF00) >> 8) == 0xA;
  if (!keyDown)
    return event;

  int qtKey = 0;
  switch (keyCode) {
  case NX_KEYTYPE_PLAY:
    qtKey = Qt::Key_MediaPlay;
    break;
  case NX_KEYTYPE_FAST:
    qtKey = Qt::Key_MediaNext;
    break;
  case NX_KEYTYPE_REWIND:
    qtKey = Qt::Key_MediaPrevious;
    break;
  default:
    return event;
  }

  QWindow *window = QGuiApplication::focusWindow();
  if (!window && !QGuiApplication::allWindows().isEmpty())
    window = QGuiApplication::allWindows().first();
  if (window) {
    QKeyEvent *press = new QKeyEvent(QEvent::KeyPress, qtKey, Qt::NoModifier);
    QCoreApplication::postEvent(window, press);
    QKeyEvent *release = new QKeyEvent(QEvent::KeyRelease, qtKey, Qt::NoModifier);
    QCoreApplication::postEvent(window, release);
  }
  return nullptr;
}

static void setupMediaKeyTap() {
  if (sEventTap)
    return;
  CGEventMask mask = CGEventMaskBit(NX_SYSDEFINED);
  sEventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, mask, mediaKeyCallback,
                               nullptr);
  if (!sEventTap)
    return;
  CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, sEventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetMain(), source, kCFRunLoopCommonModes);
  CFRelease(source);
  CGEventTapEnable(sEventTap, true);
}

static void cleanupMacIntegration() {
  if (!sEventTap)
    return;
  CFRunLoopSourceRef source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, sEventTap, 0);
  if (source) {
    CFRunLoopRemoveSource(CFRunLoopGetMain(), source, kCFRunLoopCommonModes);
    CFRelease(source);
  }
  CFMachPortInvalidate(sEventTap);
  CFRelease(sEventTap);
  sEventTap = nullptr;
}

void updateNowPlayingInfo(const MediaMetadata &meta) {
  NSMutableDictionary *info = [NSMutableDictionary dictionary];
  if (!meta.title.empty())
    info[MPMediaItemPropertyTitle] = [NSString stringWithUTF8String:meta.title.c_str()];
  if (!meta.artist.empty())
    info[MPMediaItemPropertyArtist] = [NSString stringWithUTF8String:meta.artist.c_str()];
  if (!meta.album.empty())
    info[MPMediaItemPropertyAlbumTitle] = [NSString stringWithUTF8String:meta.album.c_str()];
  if (meta.duration > 0.0)
    info[MPMediaItemPropertyPlaybackDuration] = @(meta.duration);
  [[MPNowPlayingInfoCenter defaultCenter] setNowPlayingInfo:info];
}

#include "../MediaPlayerController.h"
#include "TouchBar.h"

void connectNowPlayingInfo(mediaplayer::MediaPlayerController *controller) {
  QObject::connect(controller, &mediaplayer::MediaPlayerController::currentMetadataChanged,
                   [](const mediaplayer::MediaMetadata &meta) { updateNowPlayingInfo(meta); });
}

void setupMacIntegration(mediaplayer::MediaPlayerController *c) {
  setupMediaKeyTap();
  if (c)
    setupTouchBar(c);
  QObject::connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit,
                   []() { cleanupMacIntegration(); });
}
