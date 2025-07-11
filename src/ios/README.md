# iOS App

This directory contains the native iOS implementation written in Swift
with a small Objective-C++ bridge to the core C++ engine. The Xcode
project should add the files under `app/` and link against the
`mediaplayer_core` static library built by CMake for the `arm64` iOS
platform.

## Building

1. Open `MediaPlayerApp.xcodeproj` (create a new SwiftUI app targeting
iOS 14 or later).
2. Add the `mediaplayer_core` library output from the CMake build to the
project (either via "Add Files" or as a framework).
3. Include `MediaPlayer-Bridging-Header.h` in the project settings under
`Objective‑C Bridging Header`.
4. Ensure the `MediaPlayerBridge.mm` file is compiled as Objective-C++ by
setting its file type to `Objective-C++ Source`.

The SwiftUI views demonstrate basic playback control and can be expanded
with library browsing and settings screens as tasks are completed.

### Open URL

`OpenURLView` lets you enter an HTTP or RTSP address and stream it directly
through the core engine. Select the **Stream** tab and paste a URL.

### Gesture Settings

The **Settings** tab now includes toggles for swipe gestures and the
"shake to shuffle" feature. Disable them if they interfere with your usage.

### Voice Control

`VoiceControl.swift` implements a minimal wrapper around `SFSpeechRecognizer`
to issue voice commands such as "play", "pause" or "next". The Now Playing
screen includes a microphone button that activates recognition and dispatches
the recognized text to `MediaPlayerViewModel.handleVoiceCommand(_:)`.

The app requires the following keys in **Info.plist** so iOS prompts the user
for access to speech and Siri functionality:

```
NSSpeechRecognitionUsageDescription
NSSiriUsageDescription
```

`devops/build_ios.sh` builds the project from the command line using
`xcodebuild`.
