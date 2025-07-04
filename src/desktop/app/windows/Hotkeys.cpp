#ifdef _WIN32
#include "Hotkeys.h"
#include "../MediaPlayerController.h"
#include <QAbstractNativeEventFilter>
#include <QCoreApplication>
#include <windows.h>

class HotkeyFilter : public QAbstractNativeEventFilter {
public:
  mediaplayer::MediaPlayerController *ctrl{nullptr};
  bool nativeEventFilter(const QByteArray &type, void *message, long *result) override {
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_HOTKEY && ctrl) {
      switch (msg->wParam) {
      case 1:
        ctrl->play();
        break;
      case 2:
        ctrl->moveQueueItem(0, 0); // placeholder for next
        break;
      case 3:
        ctrl->removeFromQueue(0); // placeholder for prev
        break;
      }
      *result = 0;
      return true;
    }
    return false;
  }
};

static HotkeyFilter *g_filter = nullptr;

void initHotkeys(mediaplayer::MediaPlayerController *controller) {
  if (g_filter)
    return;
  RegisterHotKey(nullptr, 1, 0, VK_MEDIA_PLAY_PAUSE);
  RegisterHotKey(nullptr, 2, 0, VK_MEDIA_NEXT_TRACK);
  RegisterHotKey(nullptr, 3, 0, VK_MEDIA_PREV_TRACK);
  g_filter = new HotkeyFilter;
  g_filter->ctrl = controller;
  qApp->installNativeEventFilter(g_filter);
}

void cleanupHotkeys() {
  if (!g_filter)
    return;
  UnregisterHotKey(nullptr, 1);
  UnregisterHotKey(nullptr, 2);
  UnregisterHotKey(nullptr, 3);
  qApp->removeNativeEventFilter(g_filter);
  delete g_filter;
  g_filter = nullptr;
}
#endif
