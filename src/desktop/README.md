# Desktop GUI

This directory contains helpers for integrating the core modules with a Qt based user interface.

## FormatConverterQt

`FormatConverterQt` exposes the asynchronous `FormatConverter` via Qt signals for use in QML applications.

```
FormatConverterQt conv;
conv.convertAudio("input.wav", "output.mp3");
```

## LibraryQt

`LibraryQt` wraps the media library for QML. It allows running a background scan and exposes convenient methods to fetch media and playlist information.

```
LibraryQt lib;
lib.setLibrary(&db);
lib.startScan("/music");
```

In QML it can be used as:

```qml
LibraryQt {
    id: lib
    onScanProgress: console.log(current, total)
    onScanFinished: console.log("scan done")
}
```
