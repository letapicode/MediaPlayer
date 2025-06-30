# Handling Pull Request Merge Conflicts

This document expands on [merge_conflict.md](merge_conflict.md) and explains how to
review individual PRs when a giant PR has already landed.

## Workflow Summary

1. **Checkout the latest `main`** which contains the giant PR.
2. **List the open PRs** using `gh pr list` or the GitHub API.
3. For each PR:
   - Fetch the branch with `git fetch upstream <branch>`.
   - Run `git diff upstream/main..FETCH_HEAD` to inspect differences.
   - Attempt a merge with `git merge --no-commit --no-ff FETCH_HEAD` to detect conflicts.
   - Abort the merge with `git merge --abort` when conflicts are found.
4. Note any features or improvements that are missing from `main`.
5. Combine all missing pieces into a new task for Codex to implement in a single conflict-free PR.

By following this process the repository retains all useful work from individual PRs without repeatedly resolving the same conflicts.
