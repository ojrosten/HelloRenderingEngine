---
name: feedback-include-philosophy
description: Include discipline is pragmatic, not strict IWYU — transitive provision via stable project headers is acceptable; add includes only where breakage is plausible.
metadata:
  type: feedback
---

On 2026-07-15 I applied a broad IWYU-style include sweep (every stdlib name backed by a direct include). The user reverted it wholesale: "I don't think all of these are necessary. For example, in GenericResource.hpp, `<array>` is included transitively through ContextualResource.hpp... let's be more conservative, so just include the most important ones."

**Why:** the house bar for adding an include is "plausibly breaks on some platform/stdlib," not IWYU purity. Transitive provision through a *stable project header* counts as provided — e.g. `<array>` via ContextualResource.hpp (which can never drop it while `raw_indices` exists), or `<tuple>`/`<compare>` via sequoia's TypeAlgorithms.hpp (sequoia is the user's own library, so that dependency is under his control). The user also notes stdlib transitivity is "a changing landscape" — libc++ is the most sensitive/principled — and validates via cross-platform test runs rather than demanding formal self-containment.

**Outcome (2026-07-16):** the conservative application (three includes: `<string>` in Labels.hpp, Labels.hpp in GenericResource.hpp, `<functional>` in ShaderProgram.hpp) passed the full MSVC/GCC/clang-libc++ matrix — the calibration held.

**Long-term context:** the user's stated aspiration is to migrate to C++ modules "some day," which would dissolve this problem class entirely. Known blockers he named: significant modularization work on sequoia, plus mature `import std` support across the whole toolchain matrix (GCC/libstdc++ and CMake being the laggards as of 2026). Don't push modules unprompted, but treat include-hygiene questions as interim measures, not the endgame.

**How to apply:** when reviewing for missing includes, rank by realistic breakage risk and propose only the top tier: (1) names whose declaring header is *nowhere* in the include chain (the GenericResource→Labels.hpp `get_object_label` case, masked by lazy template instantiation); (2) libc++-granular gaps with no plausible provider (`<string>` behind only `<optional>`; `std::ranges::equal_to` without `<functional>`). Leave out: names guaranteed by direct project dependencies, de-facto-universal stdlib transitivity (`<filesystem>` → `<string>`), and `<cstddef>`-tier hygiene. Missing-include findings are exempt from the [[feedback-always-raise-nits]] "raise everything" rule in their *application* — raise them ranked, but expect only the top tier to be applied.
