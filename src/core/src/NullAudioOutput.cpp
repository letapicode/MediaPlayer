#include "mediaplayer/NullAudioOutput.h"

namespace mediaplayer {

void NullAudioOutput::setVolume(double volume) { m_volume = volume; }

double NullAudioOutput::volume() const { return m_volume; }

} // namespace mediaplayer
