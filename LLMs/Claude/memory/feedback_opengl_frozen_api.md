---
name: feedback-opengl-frozen-api
description: Don't flag hand-maintained tables of GL entry points (e.g. glad_ctx_member_info) as fragility risks — OpenGL is frozen.
metadata:
  type: feedback
---

When reviewing or critiquing this codebase, don't treat the hand-maintained `glad_ctx_member_info` array in `Source/avocet/OpenGL/Context/ContextBase.hpp` (or similar tables enumerating GL entry points) as a maintenance/drift risk.

**Why:** OpenGL is a frozen API — no new entry points will be added — so the table cannot silently fall out of sync with reality the way a hand-maintained list of, say, REST endpoints would. The existing `static_assert` on the stride is sufficient. The user pushed back on this framing when I raised it in a codebase review.

**How to apply:** Costs that *are* still fair game to mention: the per-TU compile-time cost of the static array being pulled into every header consumer, and any runtime cost of the offset arithmetic. The user has a longer-term plan to replace the table with C++26 reflection over `GladGLContext` once compilers support it, which would eliminate both the array and any maintenance question.
