---
name: feedback-suboptimal-not-always-intentional
description: Don't reflexively assume sub-optimal code is intentional. Flag what looks off and ask — the user makes genuine mistakes and welcomes being asked, not deferred to.
metadata:
  type: feedback
---

When code in this codebase looks sub-optimal, don't assume it must be deliberate. The user has explicitly clarified: *"some code is deliberately sub-optimal. Usually this is because I plan to refine it in future. But not always! Sometimes I simply make mistakes."*

**Why:** After several rounds of pushback in a code review where I'd flagged things that turned out to be intentional (defensive throws, tri-state enum for self-documentation, alignment plumbing exercised by tests, the binding-cache asymmetry), I started leaning into reflexive "this is probably deliberate" framing. The user gently corrected this: blanket deference is also wrong — they want genuine mistakes flagged, not papered over.

**How to apply:** Flag observations openly, but frame them as questions rather than judgments. "Is this intentional groundwork for a future lecture, or worth a fix now?" beats both "this should be cleaned up" (presumes mistake) and "this is probably deliberate" (presumes intent). The user is fully capable of answering "yes, planned" or "no, mistake — thanks." Either response is useful; pre-deciding for them is not.

**Restated emphatically by the user 2026-07-14**, immediately after the knowledge-horizon principle was recorded in [[user-teaching-project]]: *"please never assume that I have some clever plan for a big reveal. Sometimes, I do just make genuine oversights and I like you challenging me."* The two principles are held in tension deliberately: knowledge-horizon explains patterns **after** the user confirms them; it must never become a prior that pre-excuses observations. The empirical base rate backs this up — in the 2026-07 review week, deliberate-with-philosophy verdicts (GLFW static-init, warning suppression, `GetString` null) and genuine oversights caught by challenge (the LSan path — initially rejected, then vindicated; the spuriously-removed label gating; the `MasK` straggler; the `#pragma once` in a .cpp) were roughly equal in number. **Always challenge; let the user sort deliberate from oversight.**

Pair this with [[user-teaching-project]]: lecture-format constraints inform *register* (incremental, next-lecture-sized) but don't change *bar* (genuine observations are welcome).