# Desktop GUI

This directory contains helpers for integrating the core modules with a Qt based user interface.

## FormatConverterQt

`FormatConverterQt` exposes the asynchronous `FormatConverter` via Qt signals for use in QML applications.

```
FormatConverterQt conv;
conv.convertAudio("input.wav", "output.mp3");
```

## LibraryQt

`LibraryQt` wraps the media library for QML. It allows running a background scan
and exposes convenient methods to fetch media and playlist information. Queries
can be executed synchronously or asynchronously using an internal worker thread
so the UI remains responsive.

```
LibraryQt lib;
lib.setLibrary(&db);
lib.startScan("/music");
lib.asyncAllMedia();
```

In QML it can be used as:

```qml
LibraryQt {
    id: lib
    onScanProgress: console.log(current, total)
    onScanFinished: console.log("scan done")
    onAsyncAllMediaReady: console.log("loaded", media.length, "tracks")
    Component.onCompleted: lib.asyncAllMedia()
}
```

### LibraryView Example

The `LibraryView.qml` file demonstrates how to call `LibraryQt.allMedia()` and display
results in a `ListView`. Include it in your Qt Quick project after registering the
`mediaplayer_desktop` library.

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    LibraryQt { id: lib }

    ListView {
        anchors.fill: parent
        model: lib.allMedia()
        delegate: Text { text: modelData["title"] }
    }
}
```
\nThis example relates to Task #78 (Media Library View) in Tasks.MD.

## VisualizerQt

`VisualizerQt` wraps `ProjectMVisualizer` for use with Qt Quick. It exposes a
texture handle through the `texture` property so you can display the real-time
visualization inside QML. Call `setEnabled(true)` and invoke `render()` each
frame to update the texture. Use `nextPreset()` or `previousPreset()` to cycle
through projectM presets.

`ProjectMVisualizer` now accepts paths for presets and fonts so the desktop UI
can load custom collections. Expose these via `setPresetPath()` or
`setFonts()` before starting rendering.

## VisualizerItem

`VisualizerItem` is a `QQuickItem` that displays the texture produced by
`VisualizerQt`. Assign a `VisualizerQt` instance via the `visualizer`
property and set `running` to true. The item will call `render()` each
frame and update the texture on screen.

```qml
VisualizerQt { id: vis }
VisualizerItem {
    id: canvas
    anchors.fill: parent
    visualizer: vis
}
Timer {
    interval: 16; running: true; repeat: true
    onTriggered: canvas.update()
}
```

The `VisualizerView.qml` file in this directory contains a complete example with
buttons to switch presets.

