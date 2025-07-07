#Parallel Development Tasks

## Core Media Engine (C++17) ([Tasks.MD](Tasks.MD#core-media-engine-c17))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 1 | Initialize Core Engine Project Structure | done | relevant |
| 2 | Define Core Engine API (Header) | done | relevant |
| 3 | Stub Implementation of Core Classes | done | relevant |
| 4 | Build & Integration CI Setup | done | relevant |
| 5 | Integrate FFmpeg for Demuxing | done | relevant |
| 6 | Implement Audio Decoding (FFmpeg) | done | relevant |
| 7 | Implement Video Decoding (FFmpeg) | done | relevant |
| 8 | Audio/Video Synchronization | done | relevant |
| 9 | Buffering and Caching Logic | done | relevant |
| 10 | Threading and Locking | done | relevant |
| 11 | Hardware Decoding Support (Optional) | done | cross-platform hw decoder in `VideoDecoder` |
| 12 | Abstract Audio Output Interface | done | relevant |
| 13 | Audio Output – Windows (WASAPI) | done | relevant |
| 14 | Audio Output – macOS (CoreAudio) | done | relevant |
| 15 | Audio Output – Linux (PulseAudio/ALSA) | done | relevant |
| 16 | Audio Output – Android (OpenSL ES or AAudio) | done | implemented in `AudioOutputAndroid.cpp` |
| 17 | Audio Output – iOS (AVAudio) | done | implemented in `AudioOutputiOS.mm` |
| 18 | Audio Buffering & Mixing | done | ring buffer mixing implemented |
| 19 | Abstract Video Output Interface | done | relevant |
| 20 | Video Rendering – OpenGL (Desktop) | done | implemented in `src/core/src/OpenGLVideoOutput.cpp` |
| 21 | Video Rendering – DirectX (Windows optional) | done | implemented in `src/core/src/Direct3D11VideoOutput.cpp` |
| 22 | Video Rendering – Metal (macOS) | done | implemented in `src/core/src/MetalVideoOutput.mm` |
| 23 | Frame Renderer – Android (OpenGL ES) | done | implemented in `src/android/AndroidGLVideoOutput.cpp` |
| 24 | Frame Renderer – iOS (Metal/GL ES) | done | implemented in `src/core/src/MetalVideoOutput.mm` |
| 25 | Video Output Integration | done | relevant |
| 26 | Implement Play/Pause/Seek Logic | done | seek now calls `MediaDemuxer::resetEof` after `av_seek_frame` to resume demuxing |
| 27 | Track and Playlist Management (Core) | done | relevant |
| 28 | Playback State Notifications | done | relevant |
| 29 | Volume and Audio Effects | done | relevant |
| 30 | Media Metadata Extraction | done | relevant |
| 31 | Audio Format Conversion Utility | done | implemented in `src/format_conversion` |
| 32 | Video Transcoding Utility | done | implemented in `src/format_conversion` |
| 33 | Conversion Task Management | done | asynchronous `FormatConverter` |
| 34 | Integration in UI | done | CLI tool `mediaconvert` and Qt signals via `FormatConverterQt` |
| 35 | Network Stream Input Support | done | implemented via `NetworkStream` |
| 36 | YouTube Integration (Optional) | done | `YouTubeDL` helper resolves links |
| 37 | Streaming Protocols (HLS/DASH) | done | handled by `HlsStream` and `DashStream` |
| 38 | Internet Radio Streams | done | `InternetRadioStream` with ICY metadata |
| 39 | Subtitle Parser (SRT) | done | relevant |
| 40 | Subtitle Renderer/Provider | done | `SubtitleProvider` supplies cues |
| 41 | Subtitle Sync Adjustment | done | `SubtitleProvider::setOffset` |
| 42 | Subtitle Track Selection | done | decoder & API implemented |
| 43 | Audio Visualization Feed | done | PCM passed to `Visualizer::onAudioPCM` |
| 44 | Visualization API Hook | done | `Visualizer` interface implemented |
| 45 | Spectrum Analysis (optional) | done | `simpleFFT` utility + `BasicVisualizer` |
| 46 | Memory Management Audit | done | frame pool implemented |
| 47 | CPU/GPU Optimization | done | -O3 and SIMD flags added |
| 48 | Stress Testing Tools | done | stress_load_test.cpp |

## Media Library & Smart Playlists (C++17, SQLite) ([Tasks.MD](Tasks.MD#media-library-smart-playlists-c17,-sqlite))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 49 | Integrate SQLite Database | done | relevant |
| 50 | Define DB Schema | done | relevant |
| 51 | Implement Library Scanning | done | relevant |
| 52 | Metadata Extraction | done | TagLib & FFmpeg in `LibraryDB` |
| 53 | Update/Remove Entries | done | `updateMedia`/`removeMedia` APIs |
| 54 | Basic Playlist Management | done | relevant |
| 55 | Smart Playlist Criteria | done | `createSmartPlaylist` API |
| 56 | Auto Playlists (Recent, Frequent) | done | `recentlyAdded`/`mostPlayed` |
| 57 | AI Recommendations Hook | done | `AIRecommender` interface |
| 58 | Search Functionality | done | relevant |
| 59 | Rating System | done | relevant |
| 60 | Library-Core Integration | done | relevant |
| 61 | Expose Library API to UI | done | LibraryDB access via MediaPlayer |
| 62 | Threading for DB | done | relevant |
| 63 | Smart Playlist Evaluation | done | auto-updates on DB changes |

## Visualization Module (C++17 OpenGL) ([Tasks.MD](Tasks.MD#visualization-module-c17-opengl))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 64 | Include projectM Library | done | relevant |
| 65 | Initialize projectM Instance | done | relevant |
| 66 | PCM Data Feeding | done | relevant |
| 67 | OpenGL Context Sharing | done | relevant |
| 68 | Render Visualization Frame | done | relevant |
| 69 | Offscreen Rendering (Optional) | done | relevant |
| 70 | Switch Visualization Presets | done | relevant |
| 71 | Performance Optimization | done | relevant |
| 72 | Expose Visualizer to UI | done | relevant |
| 73 | Control Interface | done | relevant |

## Desktop GUI (Qt/QML, C++) ([Tasks.MD](Tasks.MD#desktop-gui-qtqml,-c))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 74 | Qt Project Initialization | done | initial scaffolding |
| 75 | Integrate Core Engine Library | done | linked core library |
| 76 | Design Main Player UI (QML) | done | basic layout |
| 77 | Implement Playback Controls (C++ Logic) | done | MediaPlayerController |
| 78 | Media Library View (QML ListView) | done | LibraryModel + view |
| 79 | Search UI | done | search bar connected |
| 80 | Playlist Management UI | done | PlaylistModel stub |
| 81 | Smart Playlist Editor | done | QML dialog |
| 82 | Settings Dialog | done | QML dialog |
| 83 | Video Player Window | done | VideoItem stub |
| 84 | Visualization Canvas | done | Visualizer integration |
| 85 | Responsive Layout | done | anchors and layouts |
| 86 | Windows Integration | done | stub file |
| 87 | macOS Integration | done | stub file |
| 88 | Linux Integration | done | stub file |
| 89 | MediaPlayer Controller Class | done | implemented |
| 90 | Expose Properties to QML | done | playing/position/volume |
| 91 | Event Handling | done | key shortcuts in QML |
| 92 | Error Handling UI | done | error signal |
| 93 | Light/Dark Theme | done | theme toggle |
| 94 | Icons and Graphics | done | resource file |
| 95 | Localization Support | done | translation file |
| 96 | Windows Installer Setup | done | NSIS script stub |
| 97 | macOS Bundle | done | package script |
| 98 | Linux Package/AppImage | done | build script |

## Android App (Kotlin + NDK) ([Tasks.MD](Tasks.MD#android-app-kotlin-ndk))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 99 | Create Android Studio Project | done | relevant |
| 100 | NDK Integration | done | relevant |
| 101 | Main Activity & Navigation | done | relevant |
| 102 | Library Fragment (RecyclerView) | done | relevant |
| 103 | Now Playing Fragment | done | relevant |
| 104 | Playlists Fragment | done | relevant |
| 105 | Settings Activity | done | relevant |
| 106 | JNI Bridge Class | done | relevant |
| 107 | JNI Implementations | done | relevant |
| 108 | Thread Management | done | relevant |
| 109 | Callbacks from Native | done | relevant |
| 110 | MediaSession & Notification | done | relevant |
| 111 | Permission Handling | done | relevant |
| 112 | Gesture Detection | done | relevant |
| 113 | Voice Control (Android) | done | relevant |
| 114 | Instrumented UI Tests | done | relevant |
| 115 | Compatibility Testing | open | relevant |

## iOS App (Swift UI/UIKit + Core bridging) ([Tasks.MD](Tasks.MD#ios-app-swift-uiuikit-core-bridging))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 116 | Create Xcode Project | done | relevant |
| 117 | Include Core Engine | done | relevant |
| 118 | Main UI (SwiftUI ContentView) | done | relevant |
| 119 | Library List View | done | relevant |
| 120 | Now Playing View | done | relevant |
| 121 | Navigation and Tab Bar | done | relevant |
| 122 | Settings UI | done | relevant |
| 123 | Bridging Header and Wrapper | done | relevant |
| 124 | Expose to Swift | done | relevant |
| 125 | Callbacks from Core | done | relevant |
| 126 | Data Flow | done | relevant |
| 127 | AVAudioSession Handling | done | relevant |
| 128 | Remote Command Center | done | relevant |
| 129 | Gesture Support | done | relevant |
| 130 | Siri / Voice | done | speech recognition button added in iOS app |
| 131 | Unit Tests (Swift) | done | relevant |
| 132 | UI Tests (XCTest/UIAutomation) | done | relevant |

## Gesture & Voice Control Module ([Tasks.MD](Tasks.MD#gesture-voice-control-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 133 | Desktop Mouse Gestures | done | implemented in MouseGestureFilter |
| 134 | Mobile Touch Gestures | done | drag gestures in iOS & Android UIs |
| 135 | Shake to Shuffle (Mobile) | done | shake detector on iOS/Android |
| 136 | Custom Gesture Mapping UI | done | gesture toggles in SettingsView |
| 137 | Integrate Vosk Speech Library (C++ or Python) | done | `VoskRecognizer.cpp` |
| 138 | Voice Command Grammar | done | `commands.json` |
| 139 | Microphone Capture (Desktop) | done | `MicrophoneInput.cpp` |
| 140 | Voice Input (Mobile) | done | `VoiceControl.kt` & `VoiceControl.swift` |
| 141 | Voice Command Processing | done | `VoiceCommandProcessor.cpp` |
| 142 | Feedback & Error Handling | done | `qml/Main.qml` overlay |
| 143 | Simulate Voice Commands | done | `tests/voice_sim/run.py` |
| 144 | Gesture Unit Tests | done | `tests/gesture/MouseGestureFilterTest.cpp` |

## AI Tagging Service (AI Metadata Module) ([Tasks.MD](Tasks.MD#ai-tagging-service-ai-metadata-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 145 | Choose AI Framework | open |  |
| 146 | Environment for AI | open |  |
| 147 | Music Genre Classification | open |  |
| 148 | Audio Fingerprinting (Song ID) | open |  |
| 149 | Speech-to-Text (for Videos) | open |  |
| 150 | Mood Detection | open |  |
| 151 | Object & Scene Recognition | open |  |
| 152 | Face Recognition (Optional) | open |  |
| 153 | Video Scene Segmentation | open | relevant |
| 154 | Service API | open | relevant |
| 155 | Asynchronous Tagging | open | relevant |
| 156 | Core–AI Communication | open | relevant |
| 157 | Update Library with Tags | open | relevant |
| 158 | Test on Sample Media | open | relevant |
| 159 | Performance Check | open | relevant |

## Streaming & Networking Module ([Tasks.MD](Tasks.MD#streaming-networking-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 160 | Open URL UI | done | Stream tab in iOS app |
| 161 | YouTube DL Integration | done | `YouTubeDL` helper in network module |
| 162 | Local DLNA/UPnP Support | open | relevant |
| 163 | HTTP Server for Remote Control | open | relevant |
| 164 | Discovery (mDNS) | open | relevant |
| 165 | Announcement & Discovery Handlers | open | relevant |
| 166 | Device Info Exchange | open | relevant |
| 167 | Sync Playback Position | open | relevant |
| 168 | Cloud Sync Option | open | relevant |
| 169 | Testing Sync | open | relevant |

## DevOps & Infrastructure ([Tasks.MD](Tasks.MD#devops-infrastructure))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 170 | Dockerfile for Core/C++ | done | Dockerfile.core added |
| 171 | Dockerfile for Qt | done | Dockerfile.qt added |
| 172 | Android Build Environment | open | relevant |
| 173 | iOS Build Setup | done | build script added in devops |
| 174 | GitHub Actions CI Workflow | done | relevant |
| 175 | Automated Tests in CI | open | relevant |
| 176 | Static Analysis & Lint | open | relevant |
| 177 | Code Formatting | done | relevant |
| 178 | Git Submodules for Libraries | open | relevant |
| 179 | Issue Templates and Contribution Guide | open | relevant |
| 180 | Merge Strategy for AI Agents | open | relevant |

## Testing & Quality Assurance ([Tasks.MD](Tasks.MD#testing-quality-assurance))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 181 | Core Unit Tests Setup | open | relevant |
| 182 | Library Module Tests | open | relevant |
| 183 | Visualization Tests | open | relevant |
| 184 | Playback Integration Test | open | relevant |
| 185 | End-to-End Script | open | relevant |
| 186 | Startup Time Test | open | relevant |
| 187 | Memory/CPU Profiling | open | relevant |
| 188 | Battery Test (Mobile) | open | relevant |
| 189 | Formats Compatibility Test | open | relevant |
| 190 | UI Responsiveness Test | open | relevant |
| 191 | Automate Regression Suite | open | relevant |

