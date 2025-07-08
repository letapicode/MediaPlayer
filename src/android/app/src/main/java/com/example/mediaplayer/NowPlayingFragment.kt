package com.example.mediaplayer

import android.content.Intent
import android.os.Bundle
import android.view.GestureDetector
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import android.view.ViewGroup
import android.widget.ImageButton
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class NowPlayingFragment : Fragment(), SurfaceHolder.Callback {

    private var surface: SurfaceView? = null
    private lateinit var detector: GestureDetector
    private lateinit var shakeDetector: ShakeDetector
    private lateinit var voiceLauncher: ActivityResultLauncher<Intent>

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?,
    ): View = inflater.inflate(R.layout.fragment_now_playing, container, false)

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        surface = view.findViewById<SurfaceView>(R.id.videoSurface).also {
            it.holder.addCallback(this)
        }

        voiceLauncher = VoiceControl.registerLauncher(this) { results ->
            results.firstOrNull()?.let { handleVoiceCommand(it) }
        }

        detector = GestureDetector(requireContext(), object : GestureDetector.SimpleOnGestureListener() {
            override fun onFling(
                e1: MotionEvent?,
                e2: MotionEvent?,
                velocityX: Float,
                velocityY: Float,
            ): Boolean {
                if (velocityX > 1000) {
                    lifecycleScope.launch(Dispatchers.IO) {
                        MediaPlayerNative.nativeSeek(0.0)
                    }
                }
                return true
            }
        })

        shakeDetector = ShakeDetector(requireContext()).also { sd ->
            sd.onShake = {
                lifecycleScope.launch(Dispatchers.IO) {
                    MediaPlayerNative.nativeEnableShuffle(true)
                }
            }
        }

        view.findViewById<ImageButton>(R.id.playPause).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativePlay() }
        }
        view.findViewById<ImageButton>(R.id.micButton).setOnClickListener {
            VoiceControl.startVoiceCommand(voiceLauncher)
        }

        MediaPlayerNative.nativeSetCallback(object : PlaybackListener {
            override fun onPlaybackFinished() {
                lifecycleScope.launch(Dispatchers.Main) {
                    Toast.makeText(requireContext(), "Playback finished", Toast.LENGTH_SHORT).show()
                }
            }

            override fun onPositionChanged(pos: Double) {}
        })

        view.setOnTouchListener { _, event -> detector.onTouchEvent(event) }
    }

    private fun handleVoiceCommand(text: String) {
        val command = text.lowercase()
        when {
            command.contains("play") && !command.contains("pause") ->
                lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativePlay() }
            command.contains("pause") ->
                lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativePause() }
            command.contains("next") ->
                lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativeNextTrack() }
            command.contains("previous") || command.contains("back") ->
                lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativePreviousTrack() }
        }
    }

    override fun onDestroyView() {
        MediaPlayerNative.nativeSetCallback(null)
        super.onDestroyView()
    }

    override fun onResume() {
        super.onResume()
        shakeDetector.start()
    }

    override fun onPause() {
        shakeDetector.stop()
        super.onPause()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(holder.surface)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(null)
    }

    override fun surfaceChanged(
        holder: SurfaceHolder,
        format: Int,
        width: Int,
        height: Int,
    ) {
    }
}
