package com.example.mediaplayer

import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.rule.ActivityTestRule
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class PlaybackTest {
    @get:Rule
    val rule = ActivityTestRule(MainActivity::class.java)

    @Test
    fun startPlayback() {
        // Simple smoke test ensuring activity launches
        InstrumentationRegistry.getInstrumentation().waitForIdleSync()
    }
}
