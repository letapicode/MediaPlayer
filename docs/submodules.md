# Working with Git Submodules

Some third-party libraries are included as git submodules under the `external/` directory. This keeps the main repository lightweight while letting you track upstream changes.

## Adding a Submodule

```bash
git submodule add <repo-url> external/<name>
```

Commit the updated `.gitmodules` file and the new directory.

## Updating Submodules

```bash
git submodule update --remote --merge
```

This pulls the latest commits from each submodule's default branch.

## Removing a Submodule

1. Delete the entry from `.gitmodules` and commit the change.
2. Remove the submodule directory and the reference in `.git/config`.
3. Run `git rm --cached external/<name>` and commit.

For more details see the [Git documentation](https://git-scm.com/book/en/v2/Git-Tools-Submodules).
