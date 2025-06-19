# Working with Codex

Codex is an AI coding agent that operates in a sandboxed environment. Each task runs with the repository preloaded and no internet access unless explicitly enabled. Codex can read and modify files and run commands such as building and testing.

## Best Practices

1. **Define Clear Tasks**: Provide focused prompts referencing files or modules. Codex performs well when it knows exactly what to change.
2. **Use AGENTS.md**: Place instructions and test commands in `AGENTS.md` so Codex knows how to format code, run tests, and follow repo conventions.
3. **Check the Terminal Output**: Codex cites terminal logs and file diffs so you can verify each step it performed.
4. **Review and Iterate**: Examine Codex’s pull requests for accuracy. Request follow-up changes if needed.

Codex accelerates routine development work while keeping you in control of the repository.
