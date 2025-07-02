# Media Library & Playlists

The library database creates `MediaItem`, `Playlist` and `PlaylistItem` tables.
The `PlaylistItem` table now enforces a `UNIQUE(playlist_id, path)` constraint.
If updating an existing database, remove duplicate entries from `PlaylistItem`
before applying the new schema.
