#!/usr/bin/env python3
import re
import sys
from pathlib import Path


def anchor(text: str) -> str:
    """Convert heading text to a GitHub anchor."""
    a = text.lower()
    a = re.sub(r"[^a-z0-9 ,&-]", "", a)
    a = a.replace("&", "")
    a = a.replace(" ", "-")
    a = re.sub(r"-+", "-", a)
    return a.strip("-")


def parse_tasks(md_path: Path):
    sections = []
    current = None
    with md_path.open(encoding="utf-8") as f:
        for line in f:
            if line.startswith("## "):
                heading = line[3:].strip()
                if heading.lower().startswith("module overview"):
                    current = None
                    continue
                current = [heading, []]
                sections.append(current)
            else:
                m = re.search(r"\*\s*\*Task:\*\s*\*\*(.+?)\*\*", line)
                if m and current:
                    current[1].append(m.group(1).strip())
    return sections


def generate(sections):
    out_lines = ["#Parallel Development Tasks", ""]
    idx = 1
    for heading, tasks in sections:
        out_lines.append(f"## {heading} ([Tasks.MD](Tasks.MD#{anchor(heading)}))")
        out_lines.append("")
        out_lines.append("| # | Task | Status | Notes |")
        out_lines.append("|-:|------|--------|-------|")
        for task in tasks:
            out_lines.append(f"| {idx} | {task} | open | |")
            idx += 1
        out_lines.append("")
    return "\n".join(out_lines)


def main():
    md_path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("Tasks.MD")
    sections = parse_tasks(md_path)
    print(generate(sections))


if __name__ == "__main__":
    main()
