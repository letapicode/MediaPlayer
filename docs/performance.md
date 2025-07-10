# Battery Performance Testing

This document outlines how to measure battery drain during audio and video playback on Android and iOS devices.

## Android

1. Build the release variant and install it with `adb`.
2. Charge the device to 100 % and unplug it.
3. Reset statistics:
   ```bash
   adb shell dumpsys batterystats --reset
   adb shell dumpsys battery unplug
   ```
4. Note the current level:
   ```bash
   adb shell dumpsys battery
   ```
5. Start Android Studio Energy Profiler or enable full history:
   ```bash
   adb shell dumpsys batterystats --enable full-wake-history
   ```
6. Play a test media file for 30 minutes.
7. After playback, read the battery level again and dump stats:
   ```bash
   adb shell dumpsys battery
   adb shell dumpsys batterystats > stats.txt
   ```
8. Compare the percentage drop to evaluate power usage.

A simple script can automate the measurement:

```bash
adb shell dumpsys batterystats --reset
adb shell dumpsys battery unplug
# launch playback here
sleep 1800
adb shell dumpsys battery
adb shell dumpsys batterystats > stats.txt
```

## iOS

1. Archive the app and deploy it to a device.
2. Charge the device fully and disconnect from power.
3. In Xcode choose **Product > Profile** and select **Energy Log**.
4. Start recording and begin playback of the sample video.
5. After 30 minutes stop the trace. Instruments displays energy impact and battery drop.
6. Export the log for later comparison.

The process can be scripted with `xcrun xctrace`:

```bash
xcrun xctrace record --template 'Energy Log' \
    --time-limit 30m --output energy_trace \
    --launch your.bundle.id
```

## Thresholds

For a 30 minute session on a typical mid‑range device:

- **Audio only:** under 3 % battery drop.
- **Video (1080p):** about 7 % drop.

Larger declines suggest a regression that should be investigated.
