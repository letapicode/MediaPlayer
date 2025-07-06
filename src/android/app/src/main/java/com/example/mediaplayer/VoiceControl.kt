package com.example.mediaplayer

import android.app.Activity
import android.content.Intent
import android.speech.RecognizerIntent
import androidx.activity.ComponentActivity
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.fragment.app.Fragment

object VoiceControl {

    fun registerLauncher(
        activity: ComponentActivity,
        callback: (List<String>) -> Unit
    ): ActivityResultLauncher<Intent> =
        activity.registerForActivityResult(
            ActivityResultContracts.StartActivityForResult()
        ) { result ->
            val results = if (result.resultCode == Activity.RESULT_OK) {
                result.data?.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS)
                    .orEmpty()
            } else {
                emptyList()
            }
            callback(results)
        }

    fun registerLauncher(
        fragment: Fragment,
        callback: (List<String>) -> Unit
    ): ActivityResultLauncher<Intent> =
        fragment.registerForActivityResult(
            ActivityResultContracts.StartActivityForResult()
        ) { result ->
            val results = if (result.resultCode == Activity.RESULT_OK) {
                result.data?.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS)
                    .orEmpty()
            } else {
                emptyList()
            }
            callback(results)
        }

    private fun createSpeechIntent(): Intent =
        Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH).apply {
            putExtra(
                RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM
            )
        }

    fun startVoiceCommand(launcher: ActivityResultLauncher<Intent>) {
        launcher.launch(createSpeechIntent())
    }
}
