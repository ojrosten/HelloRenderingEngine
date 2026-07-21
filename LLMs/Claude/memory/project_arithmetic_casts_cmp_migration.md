---
name: project_arithmetic_casts_cmp_migration
description: "checked_conversion_to's std::in_range migration landed with lecture 51; remaining constraint mismatch is deliberate homework"
metadata: 
  node_type: memory
  type: project
  originSessionId: 3816d8a6-9d3c-4fe2-9b2b-fbc96644c7d1
  modified: 2026-07-21T10:28:07.192Z
---

In lecture 48, `checked_conversion_to` was prototyped in `Tests/Core/Utilities/ArithmeticCastsFreeTest.cpp`. (During L48 *prep* the production header `Source/avocet/Core/Utilities/ArithmeticCasts.hpp` was an empty namespace, which is what an earlier version of this memory recorded — but by delivered `end_lecture_48` the implementation had already moved into the header, gaining `has_lossless_conversion_v` and a conditional `noexcept`. Verified 2026-07-10.) The `val > maxVal` mixed-sign comparison (now `ArithmeticCasts.hpp:29`) fires `-Wsign-compare` on gcc. This is **known and deliberately left in place** as the motivating "before".

**The fix landed with lecture 51** (on `end_lecture_51-expected`, 2026-07-16): the hand-rolled comparisons were replaced with a single `std::in_range<To>(val)` check and a unified "outside range [lo, hi]" message (the granular which-bound-crossed diagnostics were given up; versioned outputs updated to match). The `std::cmp_greater`/`std::cmp_less` alternative from earlier planning was not taken.

**Remaining known issue — deliberate homework:** the function's `std::integral` constraint admits char/bool types, but `std::in_range` mandates standard integer types, so lossy instantiations with e.g. `char` are now ill-formed — the constraint over-promises. The user confirmed in the L51 presentation (delivered 2026-07-21) that the homework is "refine the constraints on checked_conversion_to"; they will supply a fix at some point (see [[project_lecture_homework]]).

**Design fork in the fix (discussed post-lecture 2026-07-21, deliberately left unstated for attendees to discover):** lossless conversions from character types (e.g. `char`→`int`) work today because the `in_range` branch is discarded via `if constexpr`. So a uniform "both types shall be standard integer types" constraint would *outlaw currently-legal code*, whereas a minimal fix excludes only combinations that actually instantiate `std::in_range`. When the user's fix lands, check which side of this fork it takes before commenting.

**Why:** The migration is done — don't propose it. The constraint mismatch is intentional homework, not an oversight.
**How to apply:** Don't re-raise the constraint mismatch as a defect while it awaits the user's supplied fix; retire this memory once that fix lands. Relates to [[feedback_verified_across_compilers]] and [[feedback_suboptimal_not_always_intentional]].
