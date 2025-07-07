package com.example.mediaplayer

object MediaPlayerNative {
    init {
        System.loadLibrary("mediaplayer")
    }

    external fun nativeOpen(path: String): Boolean
    external fun nativeSetCallback(listener: PlaybackListener?)
    external fun nativePlay()
    external fun nativePause()
    external fun nativeStop()
    external fun nativeSeek(position: Double)
    external fun nativeSetSurface(surface: Any?)
    external fun nativeListMedia(): Array<String>
    external fun nativeNextTrack()
    external fun nativePreviousTrack()
    external fun nativeEnableShuffle(enabled: Boolean)

    // Calls that may block should be dispatched on Dispatchers.IO by the caller.
}
