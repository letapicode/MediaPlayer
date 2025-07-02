# Media Library & Playlists

The library database creates `MediaItem`, `Playlist` and `PlaylistItem` tables.
The `PlaylistItem` table now enforces a `UNIQUE(playlist_id, path)` constraint.
If updating an existing database, remove duplicate entries from `PlaylistItem`
before applying the new schema.

`LibraryDB` is a lightweight wrapper around SQLite that stores metadata for media files and manages user playlists. The database is created on first use and filled by scanning directories with TagLib and FFmpeg to read tags, duration and video resolution. It also records playback statistics such as play count and last played time.

## Database Schema

`LibraryDB` creates three tables:

- **MediaItem** — one row per media file.
  - `id` INTEGER PRIMARY KEY
  - `path` TEXT UNIQUE
  - `title` TEXT
  - `artist` TEXT
  - `album` TEXT
  - `duration` INTEGER
  - `width` INTEGER
  - `height` INTEGER
  - `rating` INTEGER
  - `play_count` INTEGER
  - `last_played` INTEGER
- **Playlist** — named playlists.
  - `id` INTEGER PRIMARY KEY
  - `name` TEXT UNIQUE
- **PlaylistItem** — items belonging to a playlist.
  - `playlist_id` INTEGER REFERENCES `Playlist(id)`
  - `path` TEXT REFERENCES `MediaItem(path)`
  - `position` INTEGER

## Typical Usage

```cpp
mediaplayer::LibraryDB db("library.db");
if (db.open()) {
    db.scanDirectory("/path/to/music");         // populate from files
    auto songs = db.search("Beatles");          // simple text search
    db.createPlaylist("favorites");
    for (const auto &m : songs)
        db.addToPlaylist("favorites", m.path);
    db.recordPlayback(songs.front().path);      // update play count
    db.close();
}
```

`scanDirectory` uses an SQLite UPSERT so rescanning will update metadata for
existing files automatically.

When scanning in the background with `scanDirectoryAsync` the function returns a
`std::thread`. Join this thread before destroying the `LibraryDB` object to
avoid the scan accessing a destroyed instance.

Other helpers allow updating or removing entries, setting ratings and retrieving the items of a playlist.

`LibraryDB` is now thread-safe. All database operations lock an internal mutex,
so methods such as `search` and playlist management can be called concurrently
from multiple threads without corruption.

## Dependencies and Building

`LibraryDB` relies on:

- **SQLite3** for the underlying database
- **TagLib** to read ID3/metadata from audio files
- **FFmpeg** to obtain duration and video resolution

It is built as the `mediaplayer_library` target. From the repository root:

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target mediaplayer_library
```

See [docs/building.md](../../docs/building.md) for full details, including how to compile the optional test programs.

## Tests

Several example tests under `tests/` exercise the library:

- `library_playlist_test.cpp` – playlist management
- `library_db_update_test.cpp` – adding, updating and removing entries
- `library_playback_update_test.cpp` – recording play counts
- `library_rating_test.cpp` – rating values
- `library_search_test.cpp` – search queries
- `library_video_metadata_test.cpp` – scanning duration and resolution

Enable tests with `-DBUILD_TESTS=ON` when running CMake to build these executables.

