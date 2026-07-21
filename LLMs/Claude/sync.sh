#!/usr/bin/env bash
# Sync per-session memory files from this repository into Claude Code's
# per-machine auto-memory directory.
#
# Usage:
#   ./sync.sh [--delete] <target-dir>
#
# With --delete, *.md files in the target that have no counterpart in the
# repository are removed afterwards (a true mirror — use after renaming,
# moving, or deleting memory files in the repo). CAUTION: memories Claude
# saves during a session land in the target first; --delete destroys any
# not yet copied back into the repository. Only use it when the repo is
# the up-to-date side.
#
# To find <target-dir> on a new machine, open a Claude Code session in
# this repo and ask: "Where is my auto-memory directory for this project?"
# Claude can read the path from its system prompt and reply with it.

set -euo pipefail

usage() {
    echo "Usage: $0 [--delete] <target-dir>" >&2
    echo "" >&2
    echo "  --delete      After copying, remove *.md files in <target-dir> that no" >&2
    echo "                longer exist in the repository's memory directory." >&2
    echo "  <target-dir>  Per-machine Claude Code memory directory, typically" >&2
    echo "                ~/.claude/projects/<project-slug>/memory/" >&2
    exit 1
}

delete=0
target=""
for arg in "$@"; do
    case "$arg" in
        --delete) delete=1 ;;
        -*)       usage ;;
        *)        [ -z "$target" ] || usage
                  target="$arg" ;;
    esac
done
[ -n "$target" ] || usage

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

if [ "$delete" -eq 1 ]; then
    removed=0
    while IFS= read -r -d '' f; do
        if [ ! -f "$src/$f" ]; then
            rm -v "$target/$f"
            removed=$((removed + 1))
        fi
    done < <(cd "$target" && find . -name '*.md' -print0)
    find "$target" -mindepth 1 -type d -empty -delete
    echo "Removed $removed stale file(s) from: $target"
fi
