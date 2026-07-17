#!/usr/bin/env bash
# Sync per-session memory files from this repository into Claude Code's
# per-machine auto-memory directory.
#
# Usage:
#   ./sync.sh <target-dir>
#
# To find <target-dir> on a new machine, open a Claude Code session in
# this repo and ask: "Where is my auto-memory directory for this project?"
# Claude can read the path from its system prompt and reply with it.

set -euo pipefail

if [ $# -ne 1 ]; then
    echo "Usage: $0 <target-dir>" >&2
    echo "" >&2
    echo "  <target-dir>  Per-machine Claude Code memory directory, typically" >&2
    echo "                ~/.claude/projects/<project-slug>/memory/" >&2
    exit 1
fi

target="$1"
src="$(cd "$(dirname "${BASH_SOURCE[0]}")/memory" && pwd)"

if [ ! -d "$src" ]; then
    echo "Error: source directory not found: $src" >&2
    exit 1
fi

mkdir -p "$target"
(cd "$src" && find . -name '*.md' -print0) | while IFS= read -r -d '' f; do
    mkdir -p "$target/$(dirname "$f")"
    cp -v "$src/$f" "$target/$f"
done

count=$(find "$src" -name '*.md' | wc -l)
echo
echo "Synced $count memory file(s) to: $target"
