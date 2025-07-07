#include "MouseGestureFilter.h"
#include "MediaPlayerController.h"
#include <QEvent>
#include <QMouseEvent>
#include <cmath>

using namespace mediaplayer;

MouseGestureFilter::MouseGestureFilter(MediaPlayerController *ctrl, QObject *parent)
    : QObject(parent), m_ctrl(ctrl) {}

bool MouseGestureFilter::eventFilter(QObject *obj, QEvent *event) {
  switch (event->type()) {
  case QEvent::MouseButtonPress: {
    auto *e = static_cast<QMouseEvent *>(event);
    if (e->button() == Qt::RightButton) {
      m_tracking = true;
      m_start = e->pos();
    }
    break;
  }
  case QEvent::MouseButtonRelease: {
    auto *e = static_cast<QMouseEvent *>(event);
    if (e->button() == Qt::RightButton && m_tracking) {
      QPoint delta = e->pos() - m_start;
      m_tracking = false;
      int dx = delta.x();
      int dy = delta.y();
      int absDx = std::abs(dx);
      int absDy = std::abs(dy);
      const int threshold = 40;
      if (absDx > absDy && absDx > threshold) {
        if (dx > 0)
          m_ctrl->nextTrack();
        else
          m_ctrl->previousTrack();
      } else if (absDy > absDx && absDy > threshold) {
        double vol = m_ctrl->volume();
        if (dy < 0)
          m_ctrl->setVolume(std::min(1.0, vol + 0.05));
        else
          m_ctrl->setVolume(std::max(0.0, vol - 0.05));
      }
    }
    break;
  }
  default:
    break;
  }
  return QObject::eventFilter(obj, event);
}
