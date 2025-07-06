package com.example.mediaplayer

/**
 * Listener interface for playback events coming from the native player.
 */
interface PlaybackListener {
    fun onPlaybackFinished()
    fun onPositionChanged(pos: Double)
}
