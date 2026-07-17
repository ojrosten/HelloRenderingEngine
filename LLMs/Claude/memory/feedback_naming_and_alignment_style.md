---
name: feedback-naming-and-alignment-style
description: When writing code for this project — verbose, crystal-clear names (worth it for return visits) and vertically aligned punctuation/arguments where possible.
metadata:
  type: feedback
---

Two code-style preferences the user applies when polishing code I generate (stated 2026-07-14, refining `VersionFreeTest.cpp`):

1. **Verbose, crystal-clear names over terse ones** — e.g. my test-local `enum class supported : bool` became `debug_and_labelling_supported`. His rationale: *"experience has taught me that verbosity is worth it if you're away from the code for any length of time."* He very rarely uses raw boolean arguments, preferring a little `enum class` with `bool` underlying type (cf. `attempt_to_compensate_for_driver_bugs` in CapableContext.hpp; convert with `std::to_underlying` away from the GL boundary). The verbosity can also carry semantic weight — `debug_and_labelling_supported` makes visible that the test couples two predicates sharing one threshold.

2. **Vertical alignment of punctuation/arguments where possible** — he finds lined-up code much easier to visually parse. Visible throughout the codebase (aligned `=` in preset-like blocks, aligned member declarations, aligned ctor initializers, e.g. the extra spaces aligning `debug_output_supported(version)` / `object_labels_supported(version)` in VersionFreeTest.cpp, or `,      m_VAO{...}` in Polygon.hpp).

**How to apply:** when generating code, don't economize on name length for identifiers that will be revisited (test helpers, enums, config fields); use bool-underlying enum classes instead of bool parameters; and align repeated call/declaration structure vertically when adjacent lines share shape. When in doubt, look at the surrounding file — the alignment patterns are consistent and deliberate. Relates to [[feedback-explicit-gl-underlying-type]] (spelling-as-documentation) and the broader types-as-documentation ethos.
