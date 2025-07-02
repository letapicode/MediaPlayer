# Cross-Device Sync

This module provides lightweight discovery and sync of playback position over the local network.
It uses UDP broadcast for discovery and a simple message protocol for control.

## Usage

Run `SyncService` in each instance of the player:

```cpp
mediaplayer::SyncService service("DesktopPlayer");
service.start();
service.setStatus(currentPath, currentPosition);
service.setSyncCallback([](const std::string &path, double pos) {
    // load `path` and seek to `pos`
});
```

To discover devices and send them the current status:

```cpp
mediaplayer::SyncClient client;
auto devices = client.discover();
for (const auto &d : devices) {
    client.sendSync(d, currentPath, currentPosition);
}
```

`DeviceInfo` reports the name, address and last advertised media.
