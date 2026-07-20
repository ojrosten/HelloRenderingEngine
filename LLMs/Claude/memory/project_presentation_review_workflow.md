---
name: presentation-review-workflow
description: "How to review the user's lecture decks via the Google Drive connector — text export pitfalls, link extraction, rendering slides as images"
metadata: 
  node_type: memory
  type: project
  originSessionId: 6621ea4e-fd28-4b55-8e2c-e493c82e9a89
  modified: 2026-07-20T14:51:10.624Z
---

The user's lecture decks live in Google Drive (connector available), titled "How to Build a Rendering Engine - Lecture N". Reviewing them ([[user-teaching-project]], [[live-coding-rehearsal]]):

- `read_file_content` gives slide text only — no images, no link targets — and **can scramble bullet order even on plain single-list slides** (caused a false-positive finding on Lecture 51's options slide, 2026-07-20). Never report an ordering problem from the text export alone; ask or verify visually.
- **Links:** export the deck as PDF via `download_file_content` (`exportMimeType: application/pdf`; result exceeds token limit and lands in a tool-results file — decode the base64 `content` field with PowerShell). Extract link targets by regexing the PDF bytes for `/URI\s*\((.*?)\)` (they're Google-redirect-wrapped).
- **Pictures/rendered slides:** poppler installed 2026-07-20 via `winget install oschwartz10612.Poppler`. After a Claude Code restart the Read tool renders PDF pages natively; in the install session it needed the full path `%LOCALAPPDATA%\Microsoft\WinGet\Packages\oschwartz10612.Poppler_Microsoft.Winget.Source_8wekyb3d8bbwe\poppler-25.07.0\Library\bin\pdftoppm.exe` (`-png -r 100 -f N -l N`). PNG export of a Slides file yields the first slide only.
- Code screenshots in decks can be checked against the repo (file/line numbers in compiler diagnostics matched `ArithmeticCasts.hpp` exactly for L51) — worth doing, it catches stale screenshots.

**Why:** the text export's limitations produced both a retracted finding (bullet order) and unreviewable content (flowcharts, code screenshots) until the PDF pipeline was set up.

**How to apply:** for deck reviews, do a text pass first, then render image-heavy/diagram slides to PNG for anything load-bearing, and extract links when the deck references external material.
