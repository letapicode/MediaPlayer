# Mobile Battery Test

This directory contains utilities for measuring battery drain on Android devices.

`android_battery_test.sh` pushes a media file to the device, plays it for a set
amount of time and captures `adb shell dumpsys batterystats` output.

Usage example:
```bash
./android_battery_test.sh sample.wav 60
```
The resulting `batterystats.txt` can be inspected for power consumption data.
