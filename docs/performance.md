# Performance Profiling

This guide outlines how to profile MediaPlayer on the major supported platforms. It complements the task **Memory/CPU Profiling** in [Tasks.MD](../Tasks.MD).

## Linux: Valgrind

1. Build the application with debug symbols:

   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make
   ```

2. Run the player under Valgrind to track memory usage or leaks:

   ```bash
   valgrind --tool=memcheck --leak-check=full ./mediaplayer path/to/4k_video.mp4
   ```

   Replace `path/to/4k_video.mp4` with your own 4K sample. Massif can be used instead of `memcheck` to analyze heap growth.

## macOS: Instruments

1. Build the macOS target in Xcode.
2. Open **Instruments** and choose the **Time Profiler** or **Allocations** template.
3. Launch the built `.app` from Instruments and play a 4K video or load a large playlist.
4. Inspect the recorded CPU and memory graphs for spikes.

Example command to launch Instruments manually:

```bash
open -a Instruments build/macos/MediaPlayer.app
```

## Android: Android Profiler

1. Open the Android project in Android Studio.
2. Connect a device or start an emulator.
3. Select **View > Tool Windows > Profiler** to open the Android Profiler.
4. Start profiling and then play a 4K video or load a playlist with hundreds of entries.
5. Review the Memory and CPU tabs for leaks or slow operations.

## Profiling a Large Playlist

To stress test playlist handling on Linux you can run:

```bash
valgrind --tool=memcheck ./mediaplayer path/to/playlist.m3u
```

Use a playlist containing many items (for example, thousands of tracks) and monitor the Valgrind output for warnings. On macOS and Android follow the same approach by loading the playlist while recording a profiling session.
