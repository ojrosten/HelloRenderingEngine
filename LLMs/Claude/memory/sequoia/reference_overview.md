---
name: sequoia-overview
description: Orientation hub for the sequoia dependency — what it is, layout, quality calibration, and links to the detailed sequoia memories
metadata:
  type: reference
---

**Sequoia** (vendored at `dependencies/sequoia`) is the user's own long-lived C++ library — a testing framework plus general-purpose utilities — developed over many years. Heavy use by client projects will only increase.

**Quality calibration (user's own words, 2026-07-16)**: not as polished as the rendering-engine code — "I knew a lot less, and the standard was less evolved, when I first started" — but *very well tested* and proven robust in deployment. Continuously modernized since 2018: even old files use concepts/ranges/CTAD; age shows structurally (recursive TMP, pre-`jthread` threading, dense nested-lambda option tables), not syntactically. House conventions: `m_PascalCase`, brace-init, hidden friends, enum-classes over bools, deducing-this for CRTP-free static dispatch.

**Layout** (`Source/sequoia/`): `TestFramework/` (the big one — runner [[sequoia-test-framework-core]], checks [[sequoia-checks-semantics]]); `Core/` (Object incl. the factory, Meta, DataStructures, ContainerUtilities, Concurrency, Logic — [[sequoia-core-utilities]]); `Maths/` (Graph mature and load-bearing; Geometry/Spaces churning) + `Physics/` (churning) — [[sequoia-maths-graphs-physics]]; smaller: Algorithms, Parsing (tree-structured CLI parser), Runtime (shell commands), FileSystem, Streaming, TextProcessing, PlatformSpecific. Also: `build_system/` (reusable CMake infrastructure that client projects bootstrap from), `aux_files/` (test/project templates), `TestAll/` + `Tests/` (sequoia's own ~40-suite test main), `TestFrameworkDiagnostics/`, `docs/` (generated Doxygen; the narrative source is `TestFrameworkPhilosophy.dox`).

**Signature ideas**: false-positive/false-negative test modes (the framework tests itself, with version-controlled diagnostic output as an outer regression layer); equality vs equivalence vs weak-equivalence checking with structural fallback chains; semantics checking (`check_semantics`) incl. allocation predictions across all propagation combinations; dogfooding — suites, CLI parsing, and dependency analysis all run on sequoia's own graph/tree machinery.

**Active plans** ([[sequoia-roadmap]]): retire manual test registration (reflection + `object::factory`); migrate to modules (forces prune/dependency-analysis overhaul); refresh the test-creation CLI. **Flux zones**: Maths/spaces and Physics — capture intent, not signatures.

When something needs sequoia context beyond these memories, the source is in-tree — go read it.
