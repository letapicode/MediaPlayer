# Performance and Battery Testing

This document outlines basic steps for profiling the application and measuring power usage.

## Memory and CPU Profiling

### Linux
1. Build the project with debug symbols: `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`
2. Run the player under `valgrind --tool=massif` to profile heap allocations. A helper script `tools/memory_profile.sh` automates this and writes `massif.txt`.
3. Use `valgrind --tool=callgrind` or `perf record` to capture CPU hotspots. `tools/cpu_profile.sh` wraps the callgrind invocation.
4. Visualise the results with `ms_print` for Massif or `kcachegrind` for Callgrind data.

## Startup Time

`tests/startup_time_test.cpp` measures how quickly the core player can open a
small WAV file. The test fails if initialization takes longer than two seconds.
Include it in routine regression runs to catch performance regressions.

### macOS
1. Open the Xcode project and choose Product > Profile.
2. Use the **Instruments** tool to run the **Time Profiler** and **Allocations** templates.
3. Inspect call stacks to identify heavy functions and leaks.

### Windows
1. Build with Visual Studio in Release mode with debug info.
2. Use the **Performance Profiler** (Alt+F2) to collect CPU Usage and Memory Usage traces.
3. The **Diagnostic Tools** window provides real time allocations and call graphs.

## Battery Consumption

On mobile platforms measure battery drain while playing media for a fixed duration. `tests/mobile/android_battery_test.sh` provides a basic harness for Android devices.

### Android
1. Connect the device via ADB and ensure it is fully charged.
2. Start playback then run `adb shell dumpsys batterystats --enable full` to reset stats.
3. After the test duration retrieve stats with `adb shell dumpsys batterystats --charged > stats.txt`.
4. Review the power usage section for the app's UID.

### iOS
1. Use **Xcode Instruments** with the **Energy Log** template.
2. Start recording before launching the app and stop after playback finishes.
3. Compare the energy impact graph to identify regressions.
