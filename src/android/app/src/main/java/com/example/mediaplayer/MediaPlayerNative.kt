package com.example.mediaplayer

object MediaPlayerNative {
    init {
        System.loadLibrary("mediaplayer")
    }

    external fun nativeOpen(path: String): Boolean
    external fun nativePlay()
    external fun nativePause()
    external fun nativeStop()
    external fun nativeSeek(position: Double)
    external fun nativeSetSurface(surface: Any?)
    external fun nativeListMedia(): Array<String>
}
