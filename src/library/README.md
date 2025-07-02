#Media Library &Playlists

The library database creates `MediaItem`, `Playlist` and `PlaylistItem` tables.
The `PlaylistItem` table now enforces a `UNIQUE(playlist_id, path)` constraint.
If updating an existing database, remove duplicate entries from `PlaylistItem`
before applying the new schema.

`LibraryDB` is a lightweight wrapper around SQLite that stores metadata for media files and manages user playlists. The database is created on first use and filled by scanning directories with TagLib and FFmpeg to read tags, duration and video resolution. It also records playback statistics such as play count and last played time.

By default the database operates in SQLite's WAL (write-ahead logging) mode to allow concurrent reads and writes. The `LibraryDB::close` method cleans up any `*-wal` journal file and the associated `*-shm` shared memory file when the connection is closed.

## Database Schema

`LibraryDB` creates three tables:

- **MediaItem** — one row per media file.
  - `id` INTEGER PRIMARY KEY
  - `path` TEXT UNIQUE
  - `title` TEXT
  - `artist` TEXT
  - `album` TEXT
  - `genre` TEXT
  - `duration` INTEGER
  - `width` INTEGER
  - `height` INTEGER
  - `rating` INTEGER
  - `added_date` INTEGER UNIX timestamp when the item was first added
  - `play_count` INTEGER
  - `last_played` INTEGER
- **Playlist** — named playlists.
  - `id` INTEGER PRIMARY KEY
  - `name` TEXT UNIQUE
- **PlaylistItem** — items belonging to a playlist.
- `playlist_id` INTEGER REFERENCES `Playlist(id)`
  - `path` TEXT REFERENCES `MediaItem(path)`
  - `position` INTEGER

The library creates indexes on `MediaItem(title)`, `MediaItem(artist)`,
`MediaItem(album)`, `MediaItem(genre)`, `MediaItem(added_date)` and
`MediaItem(play_count)` to speed up search and sorting queries.

Query functions such as `search` and `playlistItems` return a `MediaMetadata`
structure. This now includes a `rating` field corresponding to the column in
`MediaItem`.

## Typical Usage

```cpp
mediaplayer::LibraryDB db("library.db");
if (db.open()) {
  db.scanDirectory("/path/to/music"); // populate from files
  auto songs = db.search("Beatles");  // simple text search
  db.createPlaylist("favorites");
  for (const auto &m : songs)
    db.addToPlaylist("favorites", m.path);
  db.recordPlayback(songs.front().path); // update play count
  auto recent = db.recentlyAdded(5);     // top 5 recently played
  auto popular = db.mostPlayed(5);       // top 5 most played
  db.close();
}
```

`scanDirectory` uses an SQLite UPSERT so rescanning will update metadata for
existing files automatically. Entries whose files are missing are removed from
`MediaItem` unless cleanup is disabled.

Other helpers allow updating or removing entries, setting ratings and retrieving
the items of a playlist. Search and playlist queries return `MediaMetadata`
objects which now include a `rating` field. You can also fetch recently added
or most popular tracks via `recentlyAdded()` and `mostPlayed()`.

`LibraryDB` is now thread-safe. All database operations lock an internal mutex,
so methods such as `search` and playlist management can be called concurrently
from multiple threads without corruption. Private helpers like `playlistId()`
assume the mutex is already held by the caller.

### Asynchronous scanning

The `scanDirectoryAsync` method starts a background thread that scans files and
updates the database. It returns a `std::thread` which callers must join before
destroying the `LibraryDB` instance to avoid accessing it from a background task.

### Smart playlists

Use `createSmartPlaylist(name, filter)` to define a playlist whose contents are
determined by a filter expression (same syntax as `smartQuery`). The library
re-evaluates all smart playlists whenever metadata changes. Convenience methods
`recentlyAdded()` and `mostPlayed()` return dynamic lists of recent or popular
items without storing them as playlists.

### AI recommendations

`LibraryDB` exposes a simple interface to plug in recommendation algorithms via
`AIRecommender`. The `RandomAIRecommender` implementation included here returns
a random subset of library items. A slightly smarter stub,
`MoodAIRecommender`, picks tracks whose genre matches a given mood string.
Applications can set any recommender with `setRecommender()` and fetch
suggestions with `recommendations()`.

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

- `library_playlist_test.cpp` – playlist management and persistence
- `library_autoplaylist_test.cpp` – automatic "recent" and "popular" lists
- `library_db_update_test.cpp` – adding, updating and removing entries
- `library_playback_update_test.cpp` – recording play counts
- `library_rating_test.cpp` – rating values
- `library_search_test.cpp` – search queries
- `library_video_metadata_test.cpp` – scanning duration and resolution

Enable tests with `-DBUILD_TESTS=ON` when running CMake to build these executables.
