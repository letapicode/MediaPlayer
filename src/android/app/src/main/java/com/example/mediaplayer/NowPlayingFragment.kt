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
import androidx.fragment.app.Fragment
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class NowPlayingFragment : Fragment(), SurfaceHolder.Callback {

    private var surface: SurfaceView? = null
    private lateinit var detector: GestureDetector

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
        view.findViewById<ImageButton>(R.id.playPause).setOnClickListener {
            lifecycleScope.launch(Dispatchers.IO) {
                MediaPlayerNative.nativePlay()
            }
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
        super.onDestroyView()
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(holder.surface)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        MediaPlayerNative.nativeSetSurface(null)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
}
