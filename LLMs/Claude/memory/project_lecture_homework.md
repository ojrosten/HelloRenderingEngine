---
name: project_lecture_homework
description: Each lecture comes with homework — small deliberately-left tweaks for attendees; may or may not be noted in the commit message
metadata: 
  node_type: memory
  type: project
  originSessionId: 1955c583-9c7e-4552-a609-f3a9ca9f9955
---

Each lecture generally comes with homework (stated 2026-07-16): small tweaks/refinements deliberately left in the code for attendees to attempt themselves. The user occasionally remembers to note the homework in the commit message but often forgets, so absence of a note doesn't mean absence of homework. The user generally supplies a fix at some later point.

Known homework: the constraint mismatch in `checked_conversion_to` (`Source/avocet/Core/Utilities/ArithmeticCasts.hpp`) — `std::integral` admits char/bool types but `std::in_range` mandates standard integer types, so the constraint over-promises. Left as lecture 51 homework; see [[project_arithmetic_casts_cmp_migration]].

**Why:** A small known imperfection surviving onto a delivered lecture branch may be an intentional homework problem rather than an oversight — a third category alongside "scheduled future lecture content" and "genuine mistake."
**How to apply:** When reviewing, if a previously-discussed small improvement is conspicuously absent, ask whether it's homework before treating it as forgotten. Don't re-raise known homework items as defects while they await the user's supplied fix. As always ([[feedback_suboptimal_not_always_intentional]]), ask rather than presume. Relates to [[user_teaching_project]].

The session-start lecture-count check in CLAUDE.md hooks into this: when an unsuffixed `end_lecture_X` marks a newly delivered lecture, check the lecture's commit messages for a homework note and, if absent, ask the user what the homework was — then record it here (append to the "Known homework" list above; move items out once the user's fix lands).
