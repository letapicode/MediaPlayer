package com.example.mediaplayer

import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.action.ViewActions.click
import androidx.test.espresso.contrib.RecyclerViewActions.actionOnItemAtPosition
import androidx.test.espresso.intent.Intents.intended
import androidx.test.espresso.intent.matcher.IntentMatchers.hasComponent
import androidx.test.espresso.matcher.ViewMatchers.withId
import androidx.test.ext.junit.runners.AndroidJUnit4
import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.rule.GrantPermissionRule
import androidx.test.espresso.intent.rule.IntentsTestRule
import com.example.mediaplayer.PlaybackService
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class PlaybackTest {
    @get:Rule
    val intentsRule = IntentsTestRule(MainActivity::class.java)

    @get:Rule
    val permissionRule: GrantPermissionRule =
        GrantPermissionRule.grant(android.Manifest.permission.READ_EXTERNAL_STORAGE)

    @Test
    fun startPlayback() {
        // Simple smoke test ensuring activity launches
        InstrumentationRegistry.getInstrumentation().waitForIdleSync()
    }

    @Test
    fun tapLibraryItemStartsService() {
        onView(withId(R.id.list)).perform(
            actionOnItemAtPosition<androidx.recyclerview.widget.RecyclerView.ViewHolder>(0, click())
        )
        intended(hasComponent(PlaybackService::class.java.name))
    }
}
