---
name: sequoia-roadmap
description: The user's stated plans for sequoia — reflection-based test registration via the factory, modules migration (forcing prune overhaul), test-creation CLI refresh
metadata:
  type: project
---

The user's stated evolution plans for sequoia (told to me 2026-07-16), with code-level grounding. Sequoia is the user's own long-lived library — suggestions in these areas should align with, not fight, these plans.

**1. Retire the `TestAllMain.cpp` registration mechanism** in favor of an elegant reflection-based solution plus the existing `object::factory` ("the latter will require a bit of tweaking"). Grounding: today every test appears ~3× (header include, type name, display-name string) in a hand-maintained main; the nascent-test creation path *already* dispatches through `object::factory` + `std::visit`. Likely tweak targets: the factory constrains **all** products to be brace-initializable from one common argument list (`initializable_from` homogeneity), and names come via explicit strings or `nomenclator<T>` specializations — reflection would presumably supply names (cf. the consteval `type_name<T>()` in `Core/Meta/TypeAlgorithms.hpp`, which already sorts type lists by name) and kill the include boilerplate. The `object::suite` + `granular_filter` layer is the grouping/selection substrate that would sit on top. Related in-code notes: `test_vessel`'s manual type erasure, "variadic friends" workaround in `FreeTestCore.hpp`.

**Why:** registration boilerplate is the largest remaining manual-maintenance surface in the framework.
**How to apply:** when discussing test registration, treat the current mechanism as legacy-by-decree; improvements to it should be throwaway-cheap. Factory/suite tweaks that ease reflection-based construction are aligned with the plan. C++26 reflection is the presumed vehicle.

**2. Migrate sequoia to C++20 modules** at some point. The user explicitly notes the dependency-checking/test-selection machinery (prune) must be upgraded along with it. Grounding: `DependencyAnalyzer.cpp` is exclusively `#include`-lexing over hard-coded `.hpp/.cpp`-family extensions with same-stem pairing heuristics, a textual `--cutoff` hack, and an acknowledged lexer bug (`// TO DO: Bug here with #endif`, ~line 150). None of it can see `import`.
**How to apply:** don't propose incremental polish of the include-lexer as if it were long-lived; frame dependency-analysis observations against the coming modules rewrite.

**3. Refresh the test-creation command-line options** — the user regularly uses patterns the current options don't support well. Grounding: fixed six-species `create` matrix; wiring by textual pattern-matching edits of main cpp / common-includes / CMakeLists (`FileEditors.cpp`, breaks on reformatting); `?`-placeholder templates in `aux_files/`; ad-hoc string parsing of templated qualified names; no test removal/renaming. (Which specific unsupported patterns the user hits most is worth asking when this comes up.)

**4. `sort_nodes` has horrendous asymptotic behaviour** — acknowledged by the user (2026-07-17), on their TO-DO list to fix. Don't re-flag the complexity as a discovery; performance-sensitive suggestions built on node sorting should note the pending fix. (Related but distinct: the 2026-07-16 review's Tier 3 question about tree `prune` assuming `add_node`-built index ordering, which `sort_nodes` can violate.)

**5. Maths/spaces and Physics are in very considerable flux** — capture intent, not signatures; don't build on fine details of `Spaces.hpp`/`PhysicalValues.hpp`. See [[sequoia-maths-graphs-physics]].

General calibration from the user: sequoia is older than their newer codebases and less polished ("I knew a lot less — and the standard was less evolved — when I first started"), but very well tested and proven robust in deployment. Register for review feedback: modernization suggestions are welcome but should acknowledge the roadmap above; robustness complaints need strong evidence.
