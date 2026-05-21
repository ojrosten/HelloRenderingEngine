---
name: feedback-suboptimal-not-always-intentional
description: Don't reflexively assume sub-optimal code is intentional. Flag what looks off and ask — the user makes genuine mistakes and welcomes being asked, not deferred to.
metadata:
  type: feedback
---

When code in this codebase looks sub-optimal, don't assume it must be deliberate. The user has explicitly clarified: *"some code is deliberately sub-optimal. Usually this is because I plan to refine it in future. But not always! Sometimes I simply make mistakes."*

**Why:** After several rounds of pushback in a code review where I'd flagged things that turned out to be intentional (defensive throws, tri-state enum for self-documentation, alignment plumbing exercised by tests, the binding-cache asymmetry), I started leaning into reflexive "this is probably deliberate" framing. The user gently corrected this: blanket deference is also wrong — they want genuine mistakes flagged, not papered over.

**How to apply:** Flag observations openly, but frame them as questions rather than judgments. "Is this intentional groundwork for a future lecture, or worth a fix now?" beats both "this should be cleaned up" (presumes mistake) and "this is probably deliberate" (presumes intent). The user is fully capable of answering "yes, planned" or "no, mistake — thanks." Either response is useful; pre-deciding for them is not.

Pair this with [[user-teaching-project]]: lecture-format constraints inform *register* (incremental, next-lecture-sized) but don't change *bar* (genuine observations are welcome).