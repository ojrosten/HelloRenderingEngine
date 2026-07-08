---
name: project_arithmetic_casts_cmp_migration
description: "checked_conversion_to's -Wsign-compare site is a deliberate \"before\"; std::cmp_*/in_range fix is planned ~lecture 51"
metadata: 
  node_type: memory
  type: project
  originSessionId: 3816d8a6-9d3c-4fe2-9b2b-fbc96644c7d1
---

In lecture 48, `checked_conversion_to` is prototyped in `Tests/Core/Utilities/ArithmeticCastsFreeTest.cpp` (production header `Source/avocet/Core/Utilities/ArithmeticCasts.hpp` is intentionally still an empty namespace — groundwork). Its `val > maxVal` mixed-sign comparison fires `-Wsign-compare` on gcc. This is **known and deliberately left in place** as the motivating "before".

The user plans to fix it ~3 lectures after lecture 48 (≈ lecture 51, planned ~2026), when the implementation migrates from the test into `ArithmeticCasts.hpp`. Intended fix: `std::cmp_greater`/`std::cmp_less` (preserves the granular distinct error messages tested in `output/DiagnosticsOutput/Arithmetic_Casts/ArithmeticCastsFreeTest_Exceptions.txt`), with `std::in_range<To>(val)` shown as the one-call alternative (trade-off: loses which-bound-crossed diagnostics).

**Why:** Don't propose the cmp_*/in_range change or flag the sign-compare warning as a defect in the interim — it's scheduled lecture content, not an oversight.
**How to apply:** If reviewing `checked_conversion_to` before that lecture lands, treat the warning site and the empty header as intentional. Relates to [[feedback_verified_across_compilers]] and [[feedback_suboptimal_not_always_intentional]].
