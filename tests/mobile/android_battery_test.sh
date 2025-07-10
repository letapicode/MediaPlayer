#!/bin/bash
# Simple battery drain test for Android devices using ADB.
# Plays a provided media file for a fixed duration and collects batterystats.
# Usage: ./android_battery_test.sh <media_file> <duration_seconds>
set -e
if [ $# -lt 2 ]; then
  echo "Usage: $0 <media_file> <duration_seconds>" >&2
  exit 1
fi
MEDIA=$1
DUR=$2
adb shell dumpsys batterystats --reset
adb push "$MEDIA" /sdcard/test_media
adb shell am start -a android.intent.action.VIEW -d "file:///sdcard/test_media" -t audio/*
sleep "$DUR"
adb shell input keyevent 82 # wake device
adb shell input keyevent 111 # escape
adb shell am force-stop com.android.music
adb shell dumpsys batterystats --charged > batterystats.txt
adb shell rm /sdcard/test_media
echo "Battery stats written to batterystats.txt"
