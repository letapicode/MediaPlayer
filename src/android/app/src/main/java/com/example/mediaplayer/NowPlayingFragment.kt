package com.example.mediaplayer

import android.os.Bundle
import android.view.LayoutInflater
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.GestureDetector
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.ImageButton
import android.widget.Toast
import com.example.mediaplayer.ShakeDetector
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import com.example.mediaplayer.SpeechRecognizerManager
import com.example.mediaplayer.VoiceCommands

class NowPlayingFragment : Fragment(), SurfaceHolder.Callback {

    private var surface: SurfaceView? = null
    private lateinit var detector: GestureDetector
    private lateinit var shakeDetector: ShakeDetector
    private lateinit var speech: SpeechRecognizerManager

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        return inflater.inflate(R.layout.fragment_now_playing, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        surface = view.findViewById(R.id.videoSurface)
        surface?.holder?.addCallback(this)
        detector = GestureDetector(requireContext(), object : GestureDetector.SimpleOnGestureListener() {
            override fun onFling(e1: MotionEvent?, e2: MotionEvent?, velocityX: Float, velocityY: Float): Boolean {
                if (velocityX > 1000) {
                    lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativeSeek(0.0) }
                }
                return true
            }
        })
        shakeDetector = ShakeDetector(requireContext())
        speech = SpeechRecognizerManager(requireContext()).apply {
            onResult = { VoiceCommands.handle(requireContext(), it) }
        }
        shakeDetector.onShake = {
            lifecycleScope.launch(Dispatchers.IO) { MediaPlayerNative.nativeEnableShuffle(true) }
        }
        view.findViewById<ImageButton>(R.id.playPause).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                MediaPlayerNative.nativePlay()
            }
        }
        view.findViewById<ImageButton>(R.id.voiceCommand).setOnClickListener {
            speech.start()
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

    override fun onDestroyView() {
        MediaPlayerNative.nativeSetCallback(null)
        speech.stop()
        super.onDestroyView()
    }

    override fun onResume() {
        super.onResume()
        shakeDetector.start()
    }

    override fun onPause() {
        shakeDetector.stop()
        speech.stop()
        super.onPause()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(holder.surface)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(null)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
}
