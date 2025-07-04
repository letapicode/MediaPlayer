# Codex Guide

This repository hosts the early scaffolding for a cross-platform media player written mainly in C++17 and a few platform specific languages (Kotlin, Swift, QML). The project is organized so that individual tasks can be tackled independently by AI agents.

## Development Guidelines

- Follow the instructions in `CONTRIBUTING.md` for environment setup and coding standards. In particular use `clang-format -i` to format C++ code before committing.
- Tasks are enumerated in `parallel_tasks.md` and `Tasks.MD`. When adding new features or modules, reference the appropriate task.
- Commit logically separated changes with clear messages. Pull requests are squash-merged into `main`.
- There is currently no automated test suite or build script. Agents do not need to run tests, but should ensure modified code compiles where applicable.
- Binary assets such as images (`*.png`, `*.jpg`) are not stored in the repository. If a task requires them, describe the intended asset instead of adding the file so maintainers can create it manually.

## Repo Structure

- `src/` — module directories for the core engine, UIs, and services.
- `docs/` — supplementary documentation.
- `tests/` — placeholder for future tests (currently empty).

Codex agents can perform tasks such as implementing modules or updating documentation. Check `README.md` and `Masterplan.MD` for an overview of the project goals.
