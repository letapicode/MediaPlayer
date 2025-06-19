# Self Reflection

This repository is set up for Codex, OpenAI's software engineering agent. Codex operates in isolated workspaces loaded with the repository files and runs commands just like a developer would. It follows instructions in `AGENTS.md` files to run tests and adhere to project conventions.

When working with Codex, keep tasks well scoped. Provide context in issues or prompts and reference task numbers from `parallel_tasks.md`. Codex can run commands, generate code, and open pull requests, but it relies on the repository documentation for direction.

Codex produces verifiable output by citing files and terminal logs. Reviewing these citations is important when evaluating its work. Because the environment is isolated, ensure any dependencies or setup steps are listed in the repo or a setup script so Codex can reproduce them.

The best way to collaborate with Codex is to maintain clear documentation and small tasks. Codex excels at implementing isolated features, writing tests, or drafting documentation when given precise instructions.
