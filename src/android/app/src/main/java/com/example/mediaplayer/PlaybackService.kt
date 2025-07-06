package com.example.mediaplayer

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.Service
import android.content.Intent
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat
import androidx.media.session.MediaButtonReceiver
import androidx.media.app.NotificationCompat as MediaStyle
import androidx.media.session.MediaSessionCompat
import android.support.v4.media.session.PlaybackStateCompat
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class PlaybackService : Service() {
    private lateinit var session: MediaSessionCompat
    private val scope = CoroutineScope(Dispatchers.IO)

    override fun onCreate() {
        super.onCreate()
        session = MediaSessionCompat(this, "player")
        session.setCallback(object : MediaSessionCompat.Callback() {
            override fun onPlay() {
                scope.launch { MediaPlayerNative.nativePlay() }
                updateState(PlaybackStateCompat.STATE_PLAYING)
            }

            override fun onPause() {
                scope.launch { MediaPlayerNative.nativePause() }
                updateState(PlaybackStateCompat.STATE_PAUSED)
            }
        })
        createChannel()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        MediaButtonReceiver.handleIntent(session, intent)
        session.isActive = true
        updateNotification()
        return START_STICKY
    }

    private fun buildNotification(state: Int): Notification {
        val action = if (state == PlaybackStateCompat.STATE_PLAYING) {
            NotificationCompat.Action(
                android.R.drawable.ic_media_pause,
                "Pause",
                MediaButtonReceiver.buildMediaButtonPendingIntent(
                    this,
                    PlaybackStateCompat.ACTION_PAUSE
                )
            )
        } else {
            NotificationCompat.Action(
                android.R.drawable.ic_media_play,
                "Play",
                MediaButtonReceiver.buildMediaButtonPendingIntent(
                    this,
                    PlaybackStateCompat.ACTION_PLAY
                )
            )
        }

        return NotificationCompat.Builder(this, "player")
            .setContentTitle("Playing")
            .setSmallIcon(android.R.drawable.ic_media_play)
            .addAction(action)
            .setStyle(
                MediaStyle.MediaStyle()
                    .setMediaSession(session.sessionToken)
                    .setShowActionsInCompactView(0)
            )
            .setOngoing(state == PlaybackStateCompat.STATE_PLAYING)
            .build()
    }

    private fun updateNotification() {
        val state = session.controller.playbackState?.state
            ?: PlaybackStateCompat.STATE_NONE
        val notif = buildNotification(state)
        if (state == PlaybackStateCompat.STATE_PLAYING) {
            startForeground(1, notif)
        } else {
            val manager = getSystemService(NotificationManager::class.java)
            manager.notify(1, notif)
        }
    }

    private fun updateState(state: Int) {
        val playbackState = PlaybackStateCompat.Builder()
            .setActions(
                PlaybackStateCompat.ACTION_PLAY or PlaybackStateCompat.ACTION_PAUSE
            )
            .setState(state, PlaybackStateCompat.PLAYBACK_POSITION_UNKNOWN, 1.0f)
            .build()
        session.setPlaybackState(playbackState)
        updateNotification()
    }

    private fun createChannel() {
        if (Build.VERSION.SDK_INT >= 26) {
            val manager = getSystemService(NotificationManager::class.java)
            manager.createNotificationChannel(
                NotificationChannel("player", "Playback", NotificationManager.IMPORTANCE_LOW)
            )
        }
    }

    override fun onBind(intent: Intent?): IBinder? = null
}
