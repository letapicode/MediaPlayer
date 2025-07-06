package com.example.mediaplayer

/** Listener for playback events coming from native code. */
interface PlaybackListener {
    /** Called when playback of the current track finished. */
    fun onFinished()

    /** Called periodically with the current playback position in seconds. */
    fun onPosition(position: Double)
}
