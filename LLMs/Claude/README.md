# LLMs/Claude

Per-session memory and supporting material for working with Claude Code on this repository.

## Layout

```
LLMs/Claude/
├── README.md        ← this file
├── OPEN_ITEMS.md    ← consolidated index of every open actionable item (living document)
├── TESTIMONIAL.md   ← a testimonial about the maintainer, written by Claude (July 2026)
├── memory/          ← per-session memory files (source of truth, version-controlled)
│   ├── MEMORY.md    ← index, auto-loaded by Claude Code's memory system
│   └── *.md         ← individual memory entries (recalled selectively by description)
├── reviews/         ← durable review artifacts (full-codebase, sequoia, coverage analyses)
└── sync.sh          ← copies memory/ into the per-machine auto-memory directory
```

## How this relates to `CLAUDE.md` at the repo root

There are two tiers of LLM context in this repository:

1. **`/CLAUDE.md`** (repo root) — durable, project-wide knowledge. **Auto-loaded into every Claude Code session** in this project on any machine, no setup needed. Tight by design (always in context).
2. **`LLMs/Claude/memory/`** — finer-grained, often personal-collaboration calibration. Recalled *selectively* by Claude based on relevance to the current task. Needs a per-machine sync step (below) to be visible to Claude.

The split is deliberate: things every assistant should know on turn one go in `CLAUDE.md`; things that are useful when relevant but would just be noise if always loaded go in `memory/`.

## How memory works on a fresh machine

Claude Code's auto-memory system reads from a fixed per-machine path:

```
~/.claude/projects/<project-slug>/memory/
```

The `<project-slug>` is derived from the absolute path of this repository on that machine, so it differs between machines (e.g. `c--UnitySrc-HelloRenderingEngine` on a Windows checkout at `c:\UnitySrc\HelloRenderingEngine`, something different on Linux/macOS). Files only at `LLMs/Claude/memory/` are not auto-loaded — they have to be copied into the per-machine path.

## Setup on a new machine

1. Clone this repository.
2. Open a Claude Code session in the repo root and ask: *"Where is my auto-memory directory for this project?"* — Claude can read this from its system prompt and reply with the absolute path.
3. Run the sync script with that path:
   ```bash
   ./LLMs/Claude/sync.sh "<absolute-path-from-step-2>"
   ```
   This copies every `*.md` from `LLMs/Claude/memory/` into the target directory. With `--delete` it also removes target files that no longer exist in the repo — useful after renaming, moving, or deleting memory files repo-side, but **destroys any session-written memories not yet copied back**, so only use it when the repo is the up-to-date side.
4. Future Claude Code sessions on this machine will recall memory files selectively. The auto-loaded index (`MEMORY.md`) confirms the setup worked.

## Updating memory

When Claude saves or modifies a memory file during a session, it writes to the per-machine path (`~/.claude/projects/<slug>/memory/`), not to this repository. To make the change portable:

```bash
# Copy back from the per-machine directory into the repo.
cp "<per-machine-path>"/*.md ./LLMs/Claude/memory/
```

Then review the diff and commit. Treat `LLMs/Claude/memory/` as the version-controlled source of truth; the per-machine directory is the working copy Claude reads and writes during a session.

## What's appropriate to commit here

This repository is a public teaching project. Most memory files are fine to publish — many are themselves useful background for students reading the repo (e.g. cache rationale, deducing-this convention). A small number of entries are personal-collaboration notes that the maintainer may prefer to keep local. Review the contents of `memory/` before pushing.
