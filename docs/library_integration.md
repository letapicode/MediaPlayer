# Library Integration

The media library is implemented in the `mediaplayer_library` module. It stores
metadata in an SQLite database and exposes high level APIs for scanning
files, managing playlists and retrieving items. This module integrates with
the core playback engine and the UI layers as described below.

## Core Engine

`MediaPlayer` owns an optional pointer to `LibraryDB`. When set via
`MediaPlayer::setLibrary()`, the player automatically records playback
statistics. Each time a track begins playback, `recordPlayback()` is invoked
which updates the play count and `last_played` timestamp in the database.

When a local file is opened that does not yet exist in the library the player
launches `scanFileAsync()` so the entry is added without blocking playback.

## UI Access

The Qt frontend uses `LibraryQt` to expose the library to QML. This wrapper
forwards synchronous calls like `allMedia()` and `playlistItems()` and provides
asynchronous variants using `LibraryFacade` and `LibraryWorker` so heavy
queries do not run on the UI thread.

Example usage in QML:

```qml
LibraryQt {
    id: lib
    onAsyncAllMediaReady: console.log("Loaded", media.length, "tracks")
    Component.onCompleted: lib.asyncAllMedia()
}
```

## Smart Playlists

Smart playlists are defined with `createSmartPlaylist()` and a filter
expression. `LibraryDB` schedules background updates whenever media is
added, removed, edited or when playback stats change. A worker thread
processes these updates and refreshes the playlist contents automatically.
