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
import android.support.v4.media.session.MediaSessionCompat

class PlaybackService : Service() {
    private lateinit var session: MediaSessionCompat

    override fun onCreate() {
        super.onCreate()
        session = MediaSessionCompat(this, "player")
        createChannel()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        MediaButtonReceiver.handleIntent(session, intent)
        val notif = buildNotification()
        startForeground(1, notif)
        return START_STICKY
    }

    private fun buildNotification(): Notification {
        return NotificationCompat.Builder(this, "player")
            .setContentTitle("Playing")
            .setSmallIcon(android.R.drawable.ic_media_play)
            .setStyle(MediaStyle.MediaStyle()
                .setMediaSession(session.sessionToken))
            .build()
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
