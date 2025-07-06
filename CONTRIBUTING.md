# Contributing to MediaPlayer

Thank you for your interest in improving MediaPlayer. This document outlines how to set up your development environment, how to claim tasks, and our coding and merge workflow.

## 1. Environment Setup

The project targets Windows, macOS, Linux, Android and iOS. Below are minimal setup instructions for each platform. The general prerequisites are a C++17 compiler, CMake, Git and FFmpeg development headers.
For a step-by-step build example, including how to compile and run the test programs, see [docs/building.md](docs/building.md).

### Linux (Ubuntu 22.04)
- Install development tools and libraries:
  ```bash
  sudo apt-get update
  sudo apt-get install -y build-essential cmake git \
      libavcodec-dev libavformat-dev libswresample-dev \
      libsqlite3-dev libtag1-dev libpulse-dev libass-dev
  ```
- Optional: install Qt 6 if working on the desktop UI.

### Windows
- Install **Visual Studio Build Tools 2022** with the C++ workload.
- Ensure `git` is available (Git for Windows).
- Obtain FFmpeg development libraries or build FFmpeg with the desired codecs.
- Configure CMake to use the MSVC toolchain.

### macOS
- Install **Xcode** and its command line tools.
- Install Homebrew packages:
  ```bash
  brew install cmake ffmpeg sqlite taglib
  ```
- Optional: install Qt via the Qt online installer for desktop work.

### Android
- Install the **Android SDK** and **NDK r23+** (e.g. using Android Studio or the command‑line tools).
- Ensure `sdkmanager` is in your `PATH` to install platform tools, build tools and the NDK.
- Use CMake (via the NDK) for native builds and Gradle for the Android app.

### iOS
- Use macOS with **Xcode** installed.
- Build with `xcodebuild` or through an Xcode workspace.
- If the project uses CocoaPods or Swift Package Manager, install those tools as required.

Containers for the core engine and Qt builds are defined in the repo’s Dockerfiles (see tasks in `Tasks.MD`) and can be used to reproduce these environments.

## 2. Claiming Tasks

Tasks are organized in `parallel_tasks.md`. To claim a task:
1. Open an issue referencing the exact task heading or ID from `parallel_tasks.md`.
2. Assign the issue to yourself (or request assignment from a maintainer).
3. Create a feature branch named after the task, e.g. `feature/123-audio-decoder`.
4. When the task is complete, open a pull request referencing the issue.
5. If you edit `Tasks.MD`, regenerate `parallel_tasks.md` using
   `python tools/generate_parallel_tasks.py > parallel_tasks.md` so the task table stays in sync.

Only work on one unclaimed task per branch. This keeps merges straightforward and mirrors the parallel development model described in the task list.

## 3. Coding Standards

- **C++ Style**: Follow the LLVM coding style. A `clang-format` configuration will be provided; run `clang-format -i` on your changes before committing.
- **Other Languages**: Use the conventions of the language (Kotlin for Android, Swift for iOS, QML/JS for Qt). Keep code consistently formatted.
- Avoid unsafe functions and prefer modern C++ practices such as smart pointers and RAII.
- Write unit tests where applicable. New features should include tests if the module has a test suite.

## 4. Branch and PR Workflow

1. Fork the repository or create a feature branch from `main` if you have push access.
2. Commit logically separated changes with clear messages.
3. Ensure the project builds on your platform and run tests before opening a PR.
4. Open a pull request against `main`. Link the issue for the task you claimed.
5. At least one maintainer must review and approve the PR. CI must pass before merge.

### Merge Policy

- PRs are squash‑merged into `main` to keep history clean.
- Do not force‑push to `main`. All changes go through pull requests.
- If a PR introduces conflicts, rebase your branch onto the latest `main` before requesting review.
- Large features should be split into smaller tasks when possible to ease review and reduce merge conflicts.

By following these guidelines we can keep development organized and ensure that contributions integrate smoothly. We look forward to your pull requests!

