#include "../MediaPlayerController.h"
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

  auto *playBtn = new QWinThumbnailToolButton(thumbBar);
  playBtn->setToolTip(QObject::tr("Play"));
  playBtn->setIcon(QIcon::fromTheme("media-playback-start"));

  auto *pauseBtn = new QWinThumbnailToolButton(thumbBar);
  pauseBtn->setToolTip(QObject::tr("Pause"));
  pauseBtn->setIcon(QIcon::fromTheme("media-playback-pause"));

  auto *nextBtn = new QWinThumbnailToolButton(thumbBar);
  nextBtn->setToolTip(QObject::tr("Next"));
  nextBtn->setIcon(QIcon::fromTheme("media-skip-forward"));

  thumbBar->addButton(prevBtn);
  thumbBar->addButton(playBtn);
  thumbBar->addButton(pauseBtn);
  thumbBar->addButton(nextBtn);

  auto prop = QGuiApplication::instance()->property("playerController");
  auto controller = reinterpret_cast<MediaPlayerController *>(prop.value<quintptr>());

  if (controller) {
    QObject::connect(playBtn, &QWinThumbnailToolButton::clicked, controller,
                     &MediaPlayerController::play);
    QObject::connect(pauseBtn, &QWinThumbnailToolButton::clicked, controller,
                     &MediaPlayerController::pause);
    QObject::connect(prevBtn, &QWinThumbnailToolButton::clicked, controller,
                     [controller]() { controller->seek(controller->position() - 10); });
    QObject::connect(nextBtn, &QWinThumbnailToolButton::clicked, controller,
                     [controller]() { controller->seek(controller->position() + 10); });

    QTimer *timer = new QTimer(thumbBar);
    timer->setInterval(1000);
    QObject::connect(timer, &QTimer::timeout, [controller]() {
      if (!g_progress)
        return;
      int value = static_cast<int>(controller->position());
      if (value < 0)
        value = 0;
      if (value > 100)
        value = 100;
      g_progress->setValue(value);
    });
    timer->start();
  }
}
