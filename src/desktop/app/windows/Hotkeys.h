#pragma once
#ifdef _WIN32
namespace mediaplayer {
class MediaPlayerController;
}

void initHotkeys(mediaplayer::MediaPlayerController *controller);
void cleanupHotkeys();
#endif
