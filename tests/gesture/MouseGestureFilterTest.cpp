#include "desktop/app/MouseGestureFilter.h"
#include "desktop/app/MediaPlayerController.h"
#include <QtTest/QtTest>

using namespace mediaplayer;

class MouseGestureFilterTest : public QObject {
  Q_OBJECT
private slots:
  void swipeLeftTriggersNext();
};

void MouseGestureFilterTest::swipeLeftTriggersNext() {
  MediaPlayerController ctrl;
  MouseGestureFilter filter(&ctrl);
  QSignalSpy spy(&ctrl, &MediaPlayerController::nextTrack);
  QMouseEvent press(QEvent::MouseButtonPress, QPoint(0, 0), Qt::RightButton, Qt::RightButton,
                    Qt::NoModifier);
  filter.eventFilter(nullptr, &press);
  QMouseEvent release(QEvent::MouseButtonRelease, QPoint(-100, 0), Qt::RightButton, Qt::RightButton,
                      Qt::NoModifier);
  filter.eventFilter(nullptr, &release);
  QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(MouseGestureFilterTest)
#include "MouseGestureFilterTest.moc"
