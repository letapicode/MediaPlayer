package com.example.mediaplayer

import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

object MediaPlayerNative {
    init {
        System.loadLibrary("mediaplayer")
    }

    external fun nativeOpen(path: String): Boolean
    external fun nativeSetListener(listener: PlaybackListener?)
    external fun nativePlay()
    external fun nativePause()
    external fun nativeStop()
    external fun nativeSeek(position: Double)
    external fun nativeSetSurface(surface: Any?)
    external fun nativeListMedia(): Array<String>

    suspend fun open(path: String): Boolean =
        withContext(Dispatchers.IO) { nativeOpen(path) }
}
