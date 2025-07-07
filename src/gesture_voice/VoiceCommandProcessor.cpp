#include "VoiceCommandProcessor.h"
#include "desktop/app/MediaPlayerController.h"

using namespace mediaplayer;

VoiceCommandProcessor::VoiceCommandProcessor(MediaPlayerController *ctrl, QObject *parent)
    : QObject(parent), m_ctrl(ctrl) {}

void VoiceCommandProcessor::processText(const QString &text) {
  QString cmd = text.toLower();
  if (cmd.contains("play") && !cmd.contains("pause")) {
    m_ctrl->play();
    return;
  }
  if (cmd.contains("pause") || cmd.contains("stop")) {
    m_ctrl->pause();
    return;
  }
  if (cmd.contains("next")) {
    m_ctrl->nextTrack();
    return;
  }
  if (cmd.contains("previous") || cmd.contains("back")) {
    m_ctrl->previousTrack();
    return;
  }
  if (cmd.contains("volume up")) {
    m_ctrl->setVolume(std::min(1.0, m_ctrl->volume() + 0.1));
    return;
  }
  if (cmd.contains("volume down")) {
    m_ctrl->setVolume(std::max(0.0, m_ctrl->volume() - 0.1));
    return;
  }
  emit commandUnknown(text);
}
