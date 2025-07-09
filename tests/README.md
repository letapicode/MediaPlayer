# Tests

This directory contains simple test programs. `test_srtparser.cpp` verifies
the SRT subtitle parser. `format_conversion_test.cpp` exercises the
audio conversion utility. `library_playlist_test.cpp` checks basic
playlist management in the SQLite library. `library_smartplaylist_test.cpp`
demonstrates smart playlist creation and `library_recommender_test.cpp`
shows the AI recommendation hook. `stress_load_test.cpp` spawns
multiple `MediaPlayer` instances to stress test core playback.
`library_ftssearch_test.cpp` checks full text search queries using SQLite's
FTS5 module.

Additional tests:

- `voice_sim` contains a Python script that feeds prerecorded audio to the Vosk
  recognizer and prints the resulting player action.
- `gesture` holds Qt tests for `MouseGestureFilter`.
- `upnp_enum_test.cpp` enumerates DLNA servers on the local network.
- `sync/test_sync_local.py` exercises mDNS discovery and HTTP sync between two running instances.
