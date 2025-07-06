# Desktop GUI Implementation Tasks

This document expands the Desktop GUI section from [Tasks.MD](../Tasks.MD). Each item corresponds to a focused development unit so agents can implement them independently.

## 74. Qt Project Initialization
- Set up a Qt Quick application with CMake or qmake.
- Provide `main.cpp` launching the QML UI.
- Ensure Qt 6 environment with Qt Quick and Controls modules.

## 75. Integrate Core Engine Library
- Link the C++ core engine library.
- Include core headers and verify a call succeeds.

## 76. Design Main Player UI (QML)
- Create QML layout with play/pause controls, seek slider, volume, and artwork/video area using Qt Quick Controls 2.

## 77. Implement Playback Controls (C++ Logic)
- Expose a `MediaPlayerController` via context property.
- Connect QML button signals to core playback slots.

## 78. Media Library View (QML ListView)
- Implement `QAbstractListModel` subclass fetching items from the library database.
- Bind the model to a `ListView` or `GridView` in QML.

## 79. Search UI
- Add a search bar component.
- On text change, query the library and update the list model.

## 80. Playlist Management UI
- Provide views for playlists and the "Now Playing" queue.
- Support drag-and-drop or buttons to modify lists.

## 81. Smart Playlist Editor
- Dialog for defining rules (field, condition, value).
- Produce a filter applied to the library.

## 82. Settings Dialog
- QML dialog for preferences: audio device, theme, features, shortcuts.

## 83. Video Player Window
- Custom `QQuickItem` or window to display video frames from the core.

## 84. Visualization Canvas
- `QQuickFramebufferObject` subclass integrating the visualization module.

## 85. Responsive Layout
- Use anchors and layouts to adapt to window sizes and HiDPI.

## Platform-specific UI Adjustments

### 86. Windows Integration
- Utilize QtWinExtras for taskbar progress or thumbnail buttons.

### 87. macOS Integration
- Optional Objective-C++ hooks for touch bar and media keys.

### 88. Linux Integration
- Add MPRIS DBus interface for desktop controls.

## Core‑UI Binding

### 89. MediaPlayer Controller Class
- Singleton wrapper around core `MediaPlayer` with slots and signals exposed to QML.

### 90. Expose Properties to QML
- Use `Q_PROPERTY` for playback state, track info, and progress.

### 91. Event Handling
- Map keyboard shortcuts and other input to core actions.

### 92. Error Handling UI
- Display dialogs or notifications for errors from the core.

## UI Theming and Assets

### 93. Light/Dark Theme
- Support theme switching or follow system preference.

### 94. Icons and Graphics
- Reference open-source icon assets for controls.

### 95. Localization Support
- Provide a base `.ts` translation file for UI strings.

## Desktop App Packaging

### 96. Windows Installer Setup
- Use `windeployqt` and an NSIS or MSI script to package the app.

### 97. macOS Bundle
- Package into a `.app` using `macdeployqt`; handle signing if necessary.

### 98. Linux Package/AppImage
- Provide script for `linuxdeployqt` to create an AppImage or package.

