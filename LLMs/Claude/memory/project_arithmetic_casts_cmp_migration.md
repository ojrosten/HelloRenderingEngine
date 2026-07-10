---
name: project_arithmetic_casts_cmp_migration
description: "checked_conversion_to's -Wsign-compare site is a deliberate \"before\"; std::cmp_*/in_range fix is planned ~lecture 51"
metadata: 
  node_type: memory
  type: project
  originSessionId: 3816d8a6-9d3c-4fe2-9b2b-fbc96644c7d1
---

In lecture 48, `checked_conversion_to` was prototyped in `Tests/Core/Utilities/ArithmeticCastsFreeTest.cpp`. (During L48 *prep* the production header `Source/avocet/Core/Utilities/ArithmeticCasts.hpp` was an empty namespace, which is what an earlier version of this memory recorded — but by delivered `end_lecture_48` the implementation had already moved into the header, gaining `has_lossless_conversion_v` and a conditional `noexcept`. Verified 2026-07-10.) The `val > maxVal` mixed-sign comparison (now `ArithmeticCasts.hpp:29`) fires `-Wsign-compare` on gcc. This is **known and deliberately left in place** as the motivating "before".

The user plans to fix it ~3 lectures after lecture 48 (≈ lecture 51, planned ~2026; may slip — L50 is the unification lecture). Intended fix: `std::cmp_greater`/`std::cmp_less` (preserves the granular distinct error messages tested in `output/DiagnosticsOutput/Arithmetic_Casts/ArithmeticCastsFreeTest_Exceptions.txt`), with `std::in_range<To>(val)` shown as the one-call alternative (trade-off: loses which-bound-crossed diagnostics).

**Why:** Don't propose the cmp_*/in_range change or flag the sign-compare warning as a defect in the interim — it's scheduled lecture content, not an oversight.
**How to apply:** If reviewing `checked_conversion_to` before that lecture lands, treat the hand-rolled comparison as intentional. Relates to [[feedback_verified_across_compilers]] and [[feedback_suboptimal_not_always_intentional]].
