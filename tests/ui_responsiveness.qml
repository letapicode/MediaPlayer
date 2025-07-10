import QtQuick 2.15
import QtTest 1.2
import MediaPlayer 1.0

TestCase {
    name: "UIResponsiveness"

    Timer {
        id: checkTimer
        interval: 100
        repeat: true
        running: false
        onTriggered: {
            if (!window.visible) {
                fail("Window closed unexpectedly")
            }
        }
    }

    function test_scan_responsiveness() {
        window.visible = true
        checkTimer.start()
        libraryQt.rescan()
        wait(2000)
        checkTimer.stop()
        verify(window.visible)
    }
}

