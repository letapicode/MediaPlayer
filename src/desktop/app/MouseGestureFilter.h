#ifndef MEDIAPLAYER_MOUSEGESTUREFILTER_H
#define MEDIAPLAYER_MOUSEGESTUREFILTER_H

#include <QObject>
#include <QPoint>

namespace mediaplayer {
class MediaPlayerController;

class MouseGestureFilter : public QObject {
  Q_OBJECT
public:
  explicit MouseGestureFilter(MediaPlayerController *ctrl, QObject *parent = nullptr);

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  MediaPlayerController *m_ctrl;
  bool m_tracking{false};
  QPoint m_start;
};

} // namespace mediaplayer

#endif // MEDIAPLAYER_MOUSEGESTUREFILTER_H
