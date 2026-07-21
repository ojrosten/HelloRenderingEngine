---
name: docs-as-final-qa
description: "User writes documentation as the final QA phase of a big release — doc gaps are expected mid-cycle, but should still be raised and recorded"
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 4063fb5f-9f73-474e-943f-1680bec10281
  modified: 2026-07-21T12:45:30.415Z
---

The user deliberately defers documentation to the final quality-assurance phase of a big release (stated 2026-07-21, in the context of the sequoia allocators review). Rationale: docs written earlier tend to start or become inaccurate; by the end they've built enough understanding both to write good docs and to let the act of documenting drive further design refinements — documentation is itself a design-review instrument, not just output.

**Why:** mid-cycle doc gaps (missing contract bullets, undocumented client obligations, doc drift, absent scope statements) are usually *scheduled* work, not oversights.

**How to apply:** keep raising documentation shortcomings — the user explicitly wants them raised and recorded now, because the accumulated list feeds the documentation phase. But calibrate severity: a doc gap is rarely urgent on its own; rank it as documentation-phase input rather than a defect, and don't frame "the docs are incomplete" as a process criticism. Findings like AS2/AS5/AT2/AT5 in `LLMs/Claude/reviews/sequoia_allocators_review_2026-07-21.md` are the canonical examples ([[sequoia-review-2026-07]]).
