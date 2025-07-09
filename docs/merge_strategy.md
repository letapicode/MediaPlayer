# Merge Strategy

This project occasionally generates both a "giant" pull request covering all outstanding tasks and separate pull requests for individual tasks. The giant PR is merged first to integrate everything at once. Afterwards individual PRs may conflict with `main`.

To reconcile these differences:

1. Fetch the latest `main` branch containing the merged giant PR.
2. For each individual PR, check the diff against `main` and note any missing functionality or improvements.
3. Aggregate the missing pieces into a new task description.
4. Regenerate a final PR that incorporates those improvements without conflicts.

See [docs/merge_conflict.md](merge_conflict.md) for commands to inspect PRs and detect conflicts.
