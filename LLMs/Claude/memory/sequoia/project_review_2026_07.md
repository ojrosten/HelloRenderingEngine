---
name: sequoia-review-2026-07
description: Full sequoia review (2026-07-16) — findings live in LLMs/Claude/reviews/sequoia_review_2026-07-16.md; check it before re-raising sequoia issues
metadata:
  type: project
---

A full strengths/weaknesses/gaps review of sequoia was performed 2026-07-16 (six parallel review agents, several findings verified by compiled repros). The complete report is version-controlled at `LLMs/Claude/reviews/sequoia_review_2026-07-16.md` in the HelloRenderingEngine repo. A 2026-07-17 addendum in the same file covers graph-based testing (`StateTransitionUtilities.hpp` — slated for rename to something like graph-based-testing): findings A1–A7 plus avocet-side observations on `CapabilityManagerFreeTest`.

**Why:** avoids re-deriving or re-raising known findings; the report is the reference for "was this already flagged?".

**How to apply:** before raising any sequoia issue, check the report (and whether the user has since fixed it — the reviewed state was the working copy at repo tip 2026-03-11). Headline items: graph copy construction correct only under NRVO (Connectivity.hpp:212/1334/1348); four prune-subsystem bugs whose failure mode is silently skipping/mis-crediting tests; no CI and no TSan (the process gap); the "silent tester non-participation" design hole in the checking layer; factory/parser/FileEditors findings routed as inputs to the [[sequoia-roadmap]] rework rather than immediate fixes. Fixes happen in the sequoia project offline — watch for them arriving here via dependency updates.
