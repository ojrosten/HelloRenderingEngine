---
name: project_arithmetic_casts_cmp_migration
description: "checked_conversion_to's std::in_range migration landed with lecture 51; remaining constraint mismatch is deliberate homework"
metadata: 
  node_type: memory
  type: project
  originSessionId: 3816d8a6-9d3c-4fe2-9b2b-fbc96644c7d1
---

In lecture 48, `checked_conversion_to` was prototyped in `Tests/Core/Utilities/ArithmeticCastsFreeTest.cpp`. (During L48 *prep* the production header `Source/avocet/Core/Utilities/ArithmeticCasts.hpp` was an empty namespace, which is what an earlier version of this memory recorded — but by delivered `end_lecture_48` the implementation had already moved into the header, gaining `has_lossless_conversion_v` and a conditional `noexcept`. Verified 2026-07-10.) The `val > maxVal` mixed-sign comparison (now `ArithmeticCasts.hpp:29`) fires `-Wsign-compare` on gcc. This is **known and deliberately left in place** as the motivating "before".

**The fix landed with lecture 51** (on `end_lecture_51-expected`, 2026-07-16): the hand-rolled comparisons were replaced with a single `std::in_range<To>(val)` check and a unified "outside range [lo, hi]" message (the granular which-bound-crossed diagnostics were given up; versioned outputs updated to match). The `std::cmp_greater`/`std::cmp_less` alternative from earlier planning was not taken.

**Remaining known issue — deliberate homework:** the function's `std::integral` constraint admits char/bool types, but `std::in_range` mandates standard integer types, so lossy instantiations with e.g. `char` are now ill-formed — the constraint over-promises. The user is leaving this as a lecture 51 homework problem for attendees (see [[project_lecture_homework]]) and will supply a fix at some point.

**Why:** The migration is done — don't propose it. The constraint mismatch is intentional homework, not an oversight.
**How to apply:** Don't re-raise the constraint mismatch as a defect while it awaits the user's supplied fix; retire this memory once that fix lands. Relates to [[feedback_verified_across_compilers]] and [[feedback_suboptimal_not_always_intentional]].
