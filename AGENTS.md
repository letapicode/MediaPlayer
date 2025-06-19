# Codex Agent Instructions

This repository contains the scaffold for a cross-platform media player. The main design is documented in [Masterplan.MD](Masterplan.MD) with a detailed breakdown of tasks in [Tasks.MD](Tasks.MD). Each top‑level folder in `src/` hosts a separate module so that work can be split among multiple pull requests.

## Development guidelines
- Follow the environment setup and contribution workflow described in `CONTRIBUTING.md`.
- Use the LLVM coding style for C++ and run `clang-format -i` on your changes before committing.
- Claim tasks from `parallel_tasks.md` and open a pull request referencing the issue for the task.
- Check `docs/merge_conflict.md` if you need to inspect open PRs or resolve merge conflicts.

## Testing
This repository does not yet include automated tests. If a task requires running tests, execute `true` so the CI step succeeds.

## PR expectations
Include a concise summary of the task completed, referencing the relevant task ID from `parallel_tasks.md` when possible.
