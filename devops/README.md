# DevOps & Infrastructure

This directory contains container definitions and helper scripts for building MediaPlayer.

### Android wrapper utility

`update_android_wrapper.sh` regenerates the Gradle wrapper jar used by the Android project. Run it from the repository root when CI needs a fresh wrapper.

### iOS build helper

`build_ios.sh` invokes `xcodebuild` to compile the iOS application. It expects
an Xcode project at `src/ios/MediaPlayerApp.xcodeproj` and requires the Xcode
command line tools to be installed.

## Qt build container

`Dockerfile.qt` provides an Ubuntu image with the Qt 6 SDK installed. Use it when you want to compile the desktop application without installing Qt locally.

### Build the image

```bash
docker build -f Dockerfile.qt -t mediaplayer-qt .
```

### Build the desktop app inside the container

Running the image drops you in a shell where the project has already been configured and built with CMake. The resulting binary lives in `/build/mediaplayer_desktop_app`.

```bash
docker run --rm -it mediaplayer-qt
```

To launch the GUI on a Linux host using X11 forwarding:

```bash
docker run --rm -it \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    mediaplayer-qt /build/mediaplayer_desktop_app
```

## Android build container

`Dockerfile.android` installs the Android SDK and NDK so the app can be built without a local Android Studio install.

### Build the image

```bash
docker build -f Dockerfile.android -t mediaplayer-android .
```

Run the container to access the SDK tools or execute Gradle builds:

```bash
docker run --rm -it mediaplayer-android
```
