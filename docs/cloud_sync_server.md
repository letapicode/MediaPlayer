# Cloud Sync Server

This optional helper script implements a very small HTTP service used by
`CloudSyncService` to store the last played media for each user.
It is not meant for production use but provides a simple reference
implementation.

## Usage

1. Install the Flask package:
   ```bash
   pip install flask
   ```
2. Start the server (default port `56793`):
   ```bash
   python tools/cloud_sync_server.py [port]
   ```
3. `CloudSyncService` can then `POST /status` with JSON
   `{"user": "alice", "path": "/file.mp4", "position": 12.3}` and
   `GET /status?user=alice` to retrieve it.

The server keeps everything in memory, so restarting it will clear all
stored statuses.
