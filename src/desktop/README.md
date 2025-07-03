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
\nThis example relates to Task #182 in Tasks.MD.
