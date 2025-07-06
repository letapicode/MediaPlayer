package com.example.mediaplayer

interface PlaybackListener {
    fun onPlay() {}
    fun onPause() {}
    fun onStop() {}
    fun onFinished() {}
    fun onTrackLoaded(path: String) {}
    fun onPosition(position: Double) {}
    fun onError(message: String) {}
}
