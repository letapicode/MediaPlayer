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
    external fun nativeCurrentPath(): String?
    external fun nativeCurrentPosition(): Double
    external fun nativeDiscoverDevices(): Array<String>
    external fun nativeSendSync(address: String, port: Int, path: String, position: Double)

    // Calls that may block should be dispatched on Dispatchers.IO by the caller.
}
