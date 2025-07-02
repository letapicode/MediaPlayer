#Parallel Development Tasks

## Core Media Engine (C++17) ([Tasks.MD](../Tasks.MD#core-media-engine-c17))

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

## Media Library & Smart Playlists (C++17, SQLite) ([Tasks.MD](../Tasks.MD#media-library-smart-playlists-c17,-sqlite))

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

## Visualization Module (C++17 OpenGL) ([Tasks.MD](../Tasks.MD#visualization-module-c17-opengl))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 64 | Include projectM Library | open | relevant |
| 65 | Initialize projectM Instance | open | relevant |
| 66 | PCM Data Feeding | open | relevant |
| 67 | OpenGL Context Sharing | open | relevant |
| 68 | Render Visualization Frame | open | relevant |
| 69 | Offscreen Rendering (Optional) | open | relevant |
| 70 | Switch Visualization Presets | open | relevant |
| 71 | Performance Optimization | open | relevant |
| 72 | Expose Visualizer to UI | open | relevant |
| 73 | Control Interface | open | relevant |

## Desktop GUI (Qt/QML, C++) ([Tasks.MD](../Tasks.MD#desktop-gui-qtqml,-c))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 74 | Qt Project Initialization | open | relevant |
| 75 | Integrate Core Engine Library | open | relevant |
| 76 | Design Main Player UI (QML) | open | relevant |
| 77 | Implement Playback Controls (C++ Logic) | open | relevant |
| 78 | Media Library View (QML ListView) | open | relevant |
| 79 | Search UI | open | relevant |
| 80 | Playlist Management UI | open | relevant |
| 81 | Smart Playlist Editor | open | relevant |
| 82 | Settings Dialog | open | relevant |
| 83 | Video Player Window | open | relevant |
| 84 | Visualization Canvas | open | relevant |
| 85 | Responsive Layout | open | relevant |
| 86 | Windows Integration | open | relevant |
| 87 | macOS Integration | open | relevant |
| 88 | Linux Integration | open | relevant |
| 89 | MediaPlayer Controller Class | open | relevant |
| 90 | Expose Properties to QML | open | relevant |
| 91 | Event Handling | open | relevant |
| 92 | Error Handling UI | open | relevant |
| 93 | Light/Dark Theme | open | relevant |
| 94 | Icons and Graphics | open | relevant |
| 95 | Localization Support | open | relevant |
| 96 | Windows Installer Setup | open | relevant |
| 97 | macOS Bundle | open | relevant |
| 98 | Linux Package/AppImage | open | relevant |

## Android App (Kotlin + NDK) ([Tasks.MD](../Tasks.MD#android-app-kotlin-ndk))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 99 | Create Android Studio Project | open | relevant |
| 100 | NDK Integration | open | relevant |
| 101 | Main Activity & Navigation | open | relevant |
| 102 | Library Fragment (RecyclerView) | open | relevant |
| 103 | Now Playing Fragment | open | relevant |
| 104 | Playlists Fragment | open | relevant |
| 105 | Settings Activity | open | relevant |
| 106 | JNI Bridge Class | open | relevant |
| 107 | JNI Implementations | open | relevant |
| 108 | Thread Management | open | relevant |
| 109 | Callbacks from Native | open | relevant |
| 110 | MediaSession & Notification | open | relevant |
| 111 | Permission Handling | open | relevant |
| 112 | Gesture Detection | open | relevant |
| 113 | Voice Control (Android) | open | relevant |
| 114 | Instrumented UI Tests | open | relevant |
| 115 | Compatibility Testing | open | relevant |

## iOS App (Swift UI/UIKit + Core bridging) ([Tasks.MD](../Tasks.MD#ios-app-swift-uiuikit-core-bridging))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 116 | Create Xcode Project | open | relevant |
| 117 | Include Core Engine | open | relevant |
| 118 | Main UI (SwiftUI ContentView) | open | relevant |
| 119 | Library List View | open | relevant |
| 120 | Now Playing View | open | relevant |
| 121 | Navigation and Tab Bar | open | relevant |
| 122 | Settings UI | open | relevant |
| 123 | Bridging Header and Wrapper | open | relevant |
| 124 | Expose to Swift | open | relevant |
| 125 | Callbacks from Core | open | relevant |
| 126 | Data Flow | open | relevant |
| 127 | AVAudioSession Handling | open | relevant |
| 128 | Remote Command Center | open | relevant |
| 129 | Gesture Support | open | relevant |
| 130 | Siri / Voice | open | relevant |
| 131 | Unit Tests (Swift) | open | relevant |
| 132 | UI Tests (XCTest/UIAutomation) | open | relevant |

## Gesture & Voice Control Module ([Tasks.MD](../Tasks.MD#gesture-voice-control-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 133 | Desktop Mouse Gestures | open | relevant |
| 134 | Mobile Touch Gestures | open | relevant |
| 135 | Shake to Shuffle (Mobile) | open | relevant |
| 136 | Custom Gesture Mapping UI | open | relevant |
| 137 | Integrate Vosk Speech Library (C++ or Python) | open | relevant |
| 138 | Voice Command Grammar | open | relevant |
| 139 | Microphone Capture (Desktop) | open | relevant |
| 140 | Voice Input (Mobile) | open | relevant |
| 141 | Voice Command Processing | open | relevant |
| 142 | Feedback & Error Handling | open | relevant |
| 143 | Simulate Voice Commands | open | relevant |
| 144 | Gesture Unit Tests | open | relevant |

## AI Tagging Service (AI Metadata Module) ([Tasks.MD](../Tasks.MD#ai-tagging-service-ai-metadata-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 145 | Choose AI Framework | open | relevant |
| 146 | Environment for AI | open | relevant |
| 147 | Music Genre Classification | open | relevant |
| 148 | Audio Fingerprinting (Song ID) | open | relevant |
| 149 | Speech-to-Text (for Videos) | open | relevant |
| 150 | Mood Detection | open | relevant |
| 151 | Object & Scene Recognition | open | relevant |
| 152 | Face Recognition (Optional) | open | relevant |
| 153 | Video Scene Segmentation | open | relevant |
| 154 | Service API | open | relevant |
| 155 | Asynchronous Tagging | open | relevant |
| 156 | Core–AI Communication | open | relevant |
| 157 | Update Library with Tags | open | relevant |
| 158 | Test on Sample Media | open | relevant |
| 159 | Performance Check | open | relevant |

## Streaming & Networking Module ([Tasks.MD](../Tasks.MD#streaming-networking-module))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 160 | Open URL UI | open | relevant |
| 161 | YouTube DL Integration | done | `YouTubeDL` helper in network module |
| 162 | Local DLNA/UPnP Support | open | relevant |
| 163 | HTTP Server for Remote Control | open | relevant |
| 164 | Discovery (mDNS) | open | relevant |
| 165 | Announcement & Discovery Handlers | open | relevant |
| 166 | Device Info Exchange | open | relevant |
| 167 | Sync Playback Position | open | relevant |
| 168 | Cloud Sync Option | open | relevant |
| 169 | Testing Sync | open | relevant |

## DevOps & Infrastructure ([Tasks.MD](../Tasks.MD#devops-infrastructure))

| # | Task | Status |
|-:|------|--------|
| 170 | Dockerfile for Core/C++ | done | Dockerfile.core added |
| 171 | Dockerfile for Qt | open | relevant |
| 172 | Android Build Environment | open | relevant |
| 173 | iOS Build Setup | open | relevant |
| 174 | GitHub Actions CI Workflow | done | relevant |
| 175 | Automated Tests in CI | open | relevant |
| 176 | Static Analysis & Lint | open | relevant |
| 177 | Code Formatting | done | relevant |
| 178 | Git Submodules for Libraries | open | relevant |
| 179 | Issue Templates and Contribution Guide | open | relevant |
| 180 | Merge Strategy for AI Agents | open | relevant |

## Testing & Quality Assurance ([Tasks.MD](../Tasks.MD#testing-quality-assurance))

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
