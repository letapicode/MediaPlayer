# Project Folder Structure

This document describes the intended layout of the repository. Each directory is accompanied by a short explanation or placeholder.

```
/core/           - C++ engine
/ui/desktop/     - Qt/QML interface
/ui/android/     - Kotlin + JNI
/ui/ios/         - Swift components
/tests/          - unit/integration tests
/docs/           - planning and contribution docs
```

- **/core/** – Contains the cross-platform C++17 engine responsible for media decoding and playback. Implementations are yet to be added.
- **/ui/desktop/** – Desktop user interface built with Qt and QML. This folder will hold all desktop-specific source files and resources.
- **/ui/android/** – Android application code written in Kotlin with JNI bindings to the core engine.
- **/ui/ios/** – iOS interface components written in Swift, linked to the core through native bindings.
- **/tests/** – Placeholder for unit and integration tests across modules. Includes test media files and scripts.
- **/docs/** – Documentation for planning, contribution guidelines, and design notes. This file lives here along with other docs.

