package com.example.mediaplayer

import android.content.Context
import android.widget.Toast
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

object VoiceCommands {
    fun handle(context: Context, text: String) {
        val command = text.lowercase()
        val scope = CoroutineScope(Dispatchers.IO)
        when {
            command.contains("play") && !command.contains("pause") -> {
                scope.launch { MediaPlayerNative.nativePlay() }
                return
            }
            command.contains("pause") || command.contains("stop") -> {
                scope.launch { MediaPlayerNative.nativePause() }
                return
            }
            command.contains("next") -> {
                scope.launch { MediaPlayerNative.nativeNextTrack() }
                return
            }
            command.contains("previous") || command.contains("back") -> {
                scope.launch { MediaPlayerNative.nativePreviousTrack() }
                return
            }
            else -> {
                Toast.makeText(context, "Unrecognized: $text", Toast.LENGTH_SHORT).show()
            }
        }
    }
}
