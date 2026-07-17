---
name: feedback-tests-are-customers
description: Tests are real customers of API generality — don't flag plumbing as unused just because no production caller exercises it.
metadata:
  type: feedback
---

When reviewing this codebase, treat tests as legitimate customers of API surface area. If a constructor / parameter / overload accepts a degree of freedom (e.g. `alignment`, `colour_channels`, a configurator field) that the *production* code path doesn't currently vary, look in `Tests/` before concluding the parameter is dead plumbing.

**Why:** I flagged `alignment` threading through `unique_image → image_view → texture_configurator → PixelStorei(GL_UNPACK_ALIGNMENT)` as "machinery without a customer" because the stb-loaded path in `Image.cpp:47` forces `alignment{1}`. The user pointed out the public constructors at `Image.hpp:108` and `:115` accept arbitrary alignments, and `Tests/Core/AssetManagement/AlignmentTest.{cpp,hpp}`, `PaddedImageSizeFreeTest.{cpp,hpp}`, etc. exercise the full range. The plumbing is correct and tested — it's just that the stb branch happens to be a degenerate case.

**How to apply:** Before saying "no caller varies parameter X, simplify by removing it," grep `Tests/` for the constructor or function in question. A design that's more general than the currently-shipped production path needs, but verified by tests, is solid future-proofing — not over-engineering. This codebase in particular has a richly developed test surface (curlew + `TestingDiagnostics` files for negative tests of the framework itself), so the bar for "this is dead code" should be high. Future production work (e.g. compressed texture formats, SIMD-friendly row strides) is more likely to use the existing generality than to retrofit it after the fact.
