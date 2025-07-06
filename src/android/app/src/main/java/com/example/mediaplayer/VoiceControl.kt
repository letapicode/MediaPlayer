package com.example.mediaplayer

import android.app.Activity
import android.content.Intent
import android.speech.RecognizerIntent

object VoiceControl {
    fun startVoiceCommand(activity: Activity, requestCode: Int) {
        val intent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH)
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
            RecognizerIntent.LANGUAGE_MODEL_FREE_FORM)
        activity.startActivityForResult(intent, requestCode)
    }
}
