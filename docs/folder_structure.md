# Project Folder Structure

This document describes the intended layout of the repository. Each directory is accompanied by a short explanation or placeholder.

```
/core/        - cross-platform playback engine
/desktop/     - desktop Qt UI and helpers
/android/     - Android frontend (Kotlin)
/ios/         - iOS frontend (Swift)
/library/     - SQLite based media library
/network/     - networking utilities
/sync/        - device sync service
```

- **/core/** – Cross-platform C++17 engine with audio/video decoders, outputs and player logic already implemented.
- **/desktop/** – Qt/QML desktop interface and platform integrations.
- **/android/** – Android application using JNI to the core engine.
- **/ios/** – iOS interface components in Swift tied to the core.
- **/library/** – Database layer providing media and playlist management.
- **/network/** – Network helpers and streaming utilities.
- **/sync/** – Local network sync service.
- **/tests/** – Placeholder for unit and integration tests.
- **/docs/** – Project documentation and guides.

