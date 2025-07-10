# Performance Testing Guidance

This guide describes how to record situations where the UI may stall and how to reproduce them in an automated test environment. When lag is detected, apply the recommended threading fixes so the UI remains responsive.

## UI Stall Scenarios

- **Library scanning** – during an initial import or periodic rescan of the media library, the application may walk thousands of files. If this scanning occurs on the main thread the UI can become unresponsive.
- **Streaming over slow networks** – fetching metadata or buffering content from remote servers can block the UI when networking calls happen synchronously. This is especially visible on mobile connections or when the server is distant.

Record any additional occurrences, noting what action was being performed and whether the stall is reproducible.

## Simulating Stalls with Automated Tools

### Qt (Desktop)

Use the [QTest](https://doc.qt.io/qt-6/qtest-overview.html) module to simulate user interactions that trigger heavy operations. Example workflow:

1. Start the application in a QTest test case.
2. Programmatically invoke the library scan (e.g., clicking the "Scan" button with `QTest::mouseClick`).
3. Measure UI response by sending additional events or querying widget state while the scan runs.

This approach can also script seeking or network playback to observe delays.

### Android

For Android builds, leverage [UIAutomator](https://developer.android.com/training/testing/other-components/ui-automator) to script the same scenarios:

1. Launch the app from a test case.
2. Use `UiDevice` to tap buttons that initiate scanning or streaming playback.
3. Monitor the UI thread by checking view hierarchy updates or log messages.

UIAutomator tests can be executed on devices or emulators as part of a continuous integration job.

## Threading Fixes When Lag Is Detected

If a scenario consistently causes the UI thread to block:

- Offload long-running tasks (database scans, network requests, file I/O) to a `QThread` or `QtConcurrent` worker.
- Emit signals back to the UI thread with progress updates rather than polling from the main loop.
- Ensure asynchronous APIs are used on Android (such as `AsyncTask` or Kotlin coroutines) so operations do not run on the UI thread.
- Verify that any shared data is protected with proper synchronization primitives to avoid race conditions when moving work onto background threads.

These practices keep the interface responsive even under heavy load.
