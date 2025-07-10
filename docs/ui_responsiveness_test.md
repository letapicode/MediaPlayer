# UI Responsiveness Test

This manual test verifies that the Qt interface remains responsive during heavy
operations such as scanning the media library.

## QML Harness

`tests/ui_responsiveness.qml` contains a small QML app that triggers a library
rescan using the production models. While the scan runs it regularly queries the
`FrameAnimation` state to ensure the event loop is active. If events stop being
processed, the test fails.

Run the harness with:
```bash
qmltestrunner tests/ui_responsiveness.qml
```

The script exits with code `0` on success. Integrate this invocation into CI or
run it manually when investigating performance issues.
