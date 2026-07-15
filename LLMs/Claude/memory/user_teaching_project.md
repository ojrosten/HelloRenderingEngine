---
name: user-teaching-project
description: HelloRenderingEngine is a teaching project with accompanying lectures — code may be deliberately pedagogical rather than production-optimal.
metadata:
  type: user
---

The user maintains HelloRenderingEngine as a teaching project (see README.md line 3) with accompanying lectures — branch names like `end_lecture_46` reflect the lecture cadence. The goal is to explore modern C++ techniques (C++23, soon C++26 reflection) and OpenGL together.

**The course is specifically on *practical software engineering*, not language features in the abstract.** This colors what counts as in-scope: cross-platform compromise, fallback design, capability gating, driver-bug workarounds, dependency management, test infrastructure, and similar real-world considerations are *teaching content*, not obstacles. See [[reference-cross-platform-targets]] for how this shapes suggestions involving newer OpenGL features.

**Lecture format & cadence:** 50 lectures delivered as of 2026-07-15, each ~1 hour: ~40 min presentation + ~20 min live coding. Some infrastructure is prepared off-line, but the amount of code added per lecture is necessarily small. This means:
- The codebase grows incrementally over many small steps.
- The right register for improvement suggestions is "next-lecture-sized," not "sweeping refactor across the engine."
- Branch names like `end_lecture_NN` mark the per-lecture commit boundary and are reliable anchors for "what was the state at lecture N."

This means some code exists primarily to *demonstrate* a language feature, not because it's the production-optimal choice. The user is aware of this and explicitly addresses production trade-offs in the lectures rather than in the code itself.

**The knowledge-horizon principle (stated explicitly 2026-07-14):** the course strategy is to *(mostly) introduce things only when there's a concrete need*. The user sometimes looks ahead a little, but deliberately not far — "in real software engineering, you have a knowledge horizon and there are many issues you can't anticipate until you encounter them," and looking too far ahead would make the project an unrealistic teaching vehicle. This is the unifying principle behind many individually-confirmed patterns: `checked_conversion_to` hand-rolled before the `std::cmp_*` lecture, the binding cache growing per-resource on demonstrated need, warning suppression on demonstrated need ([[feedback-minimal-warning-suppression]]), and partial generality like `attrib_ptr_info::advance` awaiting its matrix-attribute customer (geometrical-transformations lectures). **How to apply:** when reviewing, "this machinery is more general than its emitter/customer" or "this doesn't handle case X" *may* be a scheduled encounter rather than a defect — but never presume so: raise it and ask (the user was emphatic about this — see [[feedback-suboptimal-not-always-intentional]]; genuine oversights are common and challenges are welcome). Conversely, don't propose front-running needs the code hasn't hit.

**How to apply:** When reviewing code, weight the pedagogical purpose alongside technical merit. Don't critique a construct as if it were production code if its purpose is to showcase a modern C++ feature. For example: the `std::generator` / libc++ workaround duplication in `Source/avocet/OpenGL/Debugging/Errors.cpp` is intentional — the user wouldn't ship the workaround in production but kept it to demo C++23 generators where the toolchain supports them. Frame observations about code shape as "here's a trade-off you may have already discussed in the lecture" rather than "this should be cleaned up." See [[feedback-suboptimal-not-always-intentional]] for the converse — don't over-correct into reflexive deference.
