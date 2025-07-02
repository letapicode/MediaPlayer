# Tests

This directory contains simple test programs. `test_srtparser.cpp` verifies
the SRT subtitle parser. `format_conversion_test.cpp` exercises the
audio conversion utility. `library_playlist_test.cpp` checks basic
playlist management in the SQLite library. `library_smartplaylist_test.cpp`
demonstrates smart playlist creation and `library_recommender_test.cpp`
shows the AI recommendation hook. `stress_load_test.cpp` spawns
multiple `MediaPlayer` instances to stress test core playback.
