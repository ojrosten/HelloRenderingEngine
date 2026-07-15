---
name: feedback-always-raise-nits
description: The user wants even trivial nits raised — cosmetics, stray blank lines, spacing, commit-message typos, idiom inconsistencies. Never self-censor findings as "too small to mention".
metadata:
  type: feedback
---

On 2026-07-15 (post-lecture-50 review), after acting on a string of tiny findings — a double blank line, a stray space in a friend declaration, commit-message typos, a raw-member access where the accessor is the file's idiom — the user said: "thanks for raising it and please always do! As you've probably gathered, I strive for perfection :)".

**Why:** the user's standard is perfection, and small findings are valued rather than treated as noise. Every nit raised in that session was acted on — including amending and force-pushing an already-pushed commit solely to fix one typo'd word ("non-redudant" → "now-redundant").

**How to apply:** include cosmetic/trivial observations in every review — typos in comments and commit messages, blank-line and whitespace slips, inconsistencies with a file's local idiom — ranked clearly below substantive findings so they don't obscure them. Do still distinguish slips from deliberate style: vertical alignment and verbose naming are intentional (see [[feedback-naming-and-alignment-style]]), so check whether an oddity aligns with something before flagging it.
