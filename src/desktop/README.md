# Desktop GUI

This directory contains helpers for integrating the core modules with a Qt based user interface.

## FormatConverterQt

`FormatConverterQt` exposes the asynchronous `FormatConverter` via Qt signals for use in QML applications.

```
FormatConverterQt conv;
conv.convertAudio("input.wav", "output.mp3");
```

In QML it can be used as:

```qml
FormatConverterQt {
    id: conv
    onProgressChanged: progressBar.value = progress
    onConversionFinished: console.log("done", success)
}
```
