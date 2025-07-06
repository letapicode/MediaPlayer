package com.example.mediaplayer

import android.content.Intent
import android.speech.RecognizerIntent
import androidx.activity.result.ActivityResultLauncher

object VoiceControl {
    fun startVoiceCommand(launcher: ActivityResultLauncher<Intent>) {
        val intent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH)
        intent.putExtra(
            RecognizerIntent.EXTRA_LANGUAGE_MODEL,
            RecognizerIntent.LANGUAGE_MODEL_FREE_FORM
        )
        launcher.launch(intent)
    }
}
