#include "../MediaPlayerController.h"
#include "Hotkeys.h"
#include <QGuiApplication>
#include <QIcon>
#include <QTimer>
#include <QtWinExtras/QWinTaskbarButton>
#include <QtWinExtras/QWinTaskbarProgress>
#include <QtWinExtras/QWinThumbnailToolBar>
#include <QtWinExtras/QWinThumbnailToolButton>
#include <QtWinExtras/QtWin>

using mediaplayer::MediaPlayerController;

static QWinTaskbarProgress *g_progress = nullptr;
static QWinThumbnailToolButton *g_playBtn = nullptr;
static QWinThumbnailToolButton *g_pauseBtn = nullptr;

void updateTaskbarProgress(double value) {
  if (!g_progress)
    return;
  int v = static_cast<int>(value);
  if (v < 0)
    v = 0;
  if (v > 100)
    v = 100;
  g_progress->setValue(v);
}

void updateThumbnailButtons(bool playing) {
  if (g_playBtn)
    g_playBtn->setVisible(!playing);
  if (g_pauseBtn)
    g_pauseBtn->setVisible(playing);
}

void setupWindowsIntegration() {
  if (!QtWin::isCompositionEnabled())
    return;

  QWindow *window = nullptr;
  const auto windows = QGuiApplication::allWindows();
  if (!windows.isEmpty())
    window = windows.first();
  if (!window)
    return;

  auto *taskButton = new QWinTaskbarButton(window);
  taskButton->setWindow(window);
  g_progress = taskButton->progress();
  g_progress->setVisible(true);
  g_progress->setRange(0, 100);
  g_progress->setValue(0);

  auto *thumbBar = new QWinThumbnailToolBar(window);
  thumbBar->setWindow(window);

  auto *prevBtn = new QWinThumbnailToolButton(thumbBar);
  prevBtn->setToolTip(QObject::tr("Previous"));
  prevBtn->setIcon(QIcon::fromTheme("media-skip-backward"));

  g_playBtn = new QWinThumbnailToolButton(thumbBar);
  g_playBtn->setToolTip(QObject::tr("Play"));
  g_playBtn->setIcon(QIcon::fromTheme("media-playback-start"));

  g_pauseBtn = new QWinThumbnailToolButton(thumbBar);
  g_pauseBtn->setToolTip(QObject::tr("Pause"));
  g_pauseBtn->setIcon(QIcon::fromTheme("media-playback-pause"));

  auto *nextBtn = new QWinThumbnailToolButton(thumbBar);
  nextBtn->setToolTip(QObject::tr("Next"));
  nextBtn->setIcon(QIcon::fromTheme("media-skip-forward"));

  thumbBar->addButton(prevBtn);
  thumbBar->addButton(g_playBtn);
  thumbBar->addButton(g_pauseBtn);
  thumbBar->addButton(nextBtn);

  auto prop = QGuiApplication::instance()->property("playerController");
  auto controller = reinterpret_cast<MediaPlayerController *>(prop.value<quintptr>());

  if (controller) {
    initHotkeys(controller);
    QObject::connect(g_playBtn, &QWinThumbnailToolButton::clicked, controller,
                     &MediaPlayerController::play);
    QObject::connect(g_pauseBtn, &QWinThumbnailToolButton::clicked, controller,
                     &MediaPlayerController::pause);
    QObject::connect(prevBtn, &QWinThumbnailToolButton::clicked, controller,
                     [controller]() { controller->seek(controller->position() - 10); });
    QObject::connect(nextBtn, &QWinThumbnailToolButton::clicked, controller,
                     [controller]() { controller->seek(controller->position() + 10); });

    QObject::connect(controller, &MediaPlayerController::positionChanged, [controller]() {
      if (!g_progress)
        return;
      int value = static_cast<int>(controller->position());
      if (value < 0)
        value = 0;
      if (value > 100)
        value = 100;
      g_progress->setValue(value);
    });
    QObject::connect(controller, &MediaPlayerController::playbackStateChanged, [controller]() {
      bool playing = controller->playing();
      if (g_playBtn)
        g_playBtn->setVisible(!playing);
      if (g_pauseBtn)
        g_pauseBtn->setVisible(playing);
    });
    QObject::connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit,
                     []() { cleanupHotkeys(); });
  }
}
