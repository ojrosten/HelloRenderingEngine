---
name: feedback-live-coding-rehearsal
description: User uses me as a rehearsal partner for the live-coding portion of lectures. Iterative — first attempt doesn't need to be perfect; we fine-tune together.
metadata:
  type: feedback
---

The user occasionally runs rehearsals before delivering a lecture where they give me an instruction and ask me to perform the live-coding portion. First confirmed on 2026-05-28 for lecture 47 (`-preliminary` stage).

**Workflow shape:**
- The instruction is goal-oriented, not step-by-step. The user gives a single live-coding goal that *they themselves* will type in ~20 min during the lecture.
- It is explicitly a rehearsal — first attempt doesn't need to be perfect. User: *"this is a rehearsal so don't worry! We can fine-tune to get it perfect."*
- The lecture register applies: incremental, lecture-sized (~20 min worth of typed code), pedagogically clean, demonstrating the *technique being taught* rather than the maximally clever solution. See [[user-teaching-project]] for the broader format.

**Why:** Knowing this is a rehearsal frames the bar correctly. Production-grade and academic-perfect are *both* wrong targets. The right target is "what would actually be presentable on a live-coding stream" — small, clear, recoverable if something goes wrong mid-typing, and consistent with the codebase's existing conventions ([[feedback-deducing-this-convention]], [[reference-cross-platform-targets]], [[project-binding-cache-rationale]]).

**How to apply:**
- When asked to complete live coding, ask one clarifying question only if scope is genuinely ambiguous, then proceed.
- Don't over-engineer or chase optimizations that won't fit the time budget.
- Don't add code comments that explain what the code does — the user's lecture narration is the commentary, not the code itself.
- Welcome the fine-tuning loop. First pass is a starting point. After completion, a brief "anything you'd want different before recording?" opens the door without forcing it.
- Match existing-file style closely — formatting, naming, ordering. A rehearsal isn't the time to introduce stylistic variation.
