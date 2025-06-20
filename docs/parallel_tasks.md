# Parallel Development Tasks

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
| 8 | Audio/Video Synchronization | open | relevant |
| 9 | Buffering and Caching Logic | open | relevant |
| 10 | Threading and Locking | open | relevant |
| 11 | Hardware Decoding Support (Optional) | open | relevant |
| 12 | Abstract Audio Output Interface | done | relevant |
| 13 | Audio Output – Windows (WASAPI) | open | relevant |
| 14 | Audio Output – macOS (CoreAudio) | open | relevant |
| 15 | Audio Output – Linux (PulseAudio/ALSA) | done | relevant |
| 16 | Audio Output – Android (OpenSL ES or AAudio) | open | relevant |
| 17 | Audio Output – iOS (AVAudio) | open | relevant |
| 18 | Audio Buffering & Mixing | open | relevant |
| 19 | Abstract Video Output Interface | open | relevant |
| 20 | Video Rendering – OpenGL (Desktop) | open | relevant |
| 21 | Video Rendering – DirectX (Windows optional) | open | relevant |
| 22 | Video Rendering – Metal (macOS) | open | relevant |
| 23 | Frame Renderer – Android (OpenGL ES) | open | relevant |
| 24 | Frame Renderer – iOS (Metal/GL ES) | open | relevant |
| 25 | Video Output Integration | open | relevant |
| 26 | Implement Play/Pause/Seek Logic | open | relevant |
| 27 | Track and Playlist Management (Core) | open | relevant |
| 28 | Playback State Notifications | open | relevant |
| 29 | Volume and Audio Effects | open | relevant |
| 30 | Media Metadata Extraction | open | relevant |
| 31 | Audio Format Conversion Utility | open | relevant |
| 32 | Video Transcoding Utility | open | relevant |
| 33 | Conversion Task Management | open | relevant |
| 34 | Integration in UI | open | relevant |
| 35 | Network Stream Input Support | open | relevant |
| 36 | YouTube Integration (Optional) | open | relevant |
| 37 | Streaming Protocols (HLS/DASH) | open | relevant |
| 38 | Internet Radio Streams | open | relevant |
| 39 | Subtitle Parser (SRT) | open | relevant |
| 40 | Subtitle Renderer/Provider | open | relevant |
| 41 | Subtitle Sync Adjustment | open | relevant |
| 42 | Subtitle Track Selection | open | relevant |
| 43 | Audio Visualization Feed | open | relevant |
| 44 | Visualization API Hook | open | relevant |
| 45 | Spectrum Analysis (optional) | open | relevant |
| 46 | Memory Management Audit | open | relevant |
| 47 | CPU/GPU Optimization | open | relevant |
| 48 | Stress Testing Tools | open | relevant |

## Media Library & Smart Playlists (C++17, SQLite) ([Tasks.MD](../Tasks.MD#media-library-smart-playlists-c17,-sqlite))

| # | Task | Status | Notes |
|-:|------|--------|-------|
| 49 | Integrate SQLite Database | done | relevant |
| 50 | Define DB Schema | done | relevant |
| 51 | Implement Library Scanning | done | relevant |
| 52 | Metadata Extraction | open | relevant |
| 53 | Update/Remove Entries | open | relevant |
| 54 | Basic Playlist Management | open | relevant |
| 55 | Smart Playlist Criteria | open | relevant |
| 56 | Auto Playlists (Recent, Frequent) | open | relevant |
| 57 | AI Recommendations Hook | open | relevant |
| 58 | Search Functionality | open | relevant |
| 59 | Rating System | open | relevant |
| 60 | Library-Core Integration | open | relevant |
| 61 | Expose Library API to UI | open | relevant |
| 62 | Threading for DB | open | relevant |
| 63 | Smart Playlist Evaluation | open | relevant |

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
| 161 | YouTube DL Integration | open | relevant |
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
| 170 | Dockerfile for Core/C++ | open | relevant |
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
