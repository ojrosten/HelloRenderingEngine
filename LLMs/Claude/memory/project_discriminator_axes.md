---
name: avocet-discriminator-axes
description: "summary_discriminator vs output_discriminator — two independent axes (which checks run vs what the laid-down text says), sequoia detection-by-concept mechanics, curlew selectivity/specificity semantics"
metadata: 
  node_type: memory
  type: project
  originSessionId: f58cc83f-8059-4d66-a2ba-ed07ba4821c1
  modified: 2026-07-21T11:01:36.852Z
---

Verified against source 2026-07-21. The two per-test discriminator hooks solve *different* platform-variation problems; conflating them is the natural mistake.

**Sequoia mechanics** — both hooks are optional, detected by concept (`has_discriminated_output_v` / `has_discriminated_summary_v`, `FreeTestCore.hpp:198-209`), harvested via `get_output_discriminator` / `get_reduction_discriminator` (`TestRunner.hpp:230-244`; note the summary hook's runner-side name is "reduction") and threaded into `test_base`:

- **`output_discriminator()`** → `individual_diagnostics_paths` → `versioned_diagnostics` (`IndividualTestPaths.cpp:43`): filename `<SourceStem>_<FP|FN|>{Output,Exceptions}[_<disc>].txt` under `output/DiagnosticsOutput/<Suite_underscored>/`. Governs the two *versioned diagnostics* files: recorded failure text and `check_exception_thrown` messages ([[avocet-exception-message-stability]]).
- **`summary_discriminator()`** → `test_summary_path` → `test_summary_filename` (`IndividualTestPaths.cpp:58`): source-relative path re-rooted under `output/TestSummaries/`, stem suffixed `_<disc>`. Content is `summarize(summary, failure_messages)` (`TestRunner.cpp:253`): test name + per-category **check counts** and failures (a zero-check run writes just the name). Tests sharing a summary file append rather than overwrite (`m_FilesWrittenTo`).

**The two axes are genuinely independent:**

- **Summary/selectivity forks the check-count record.** Needed exactly when *which checks run* is platform-dependent (capability gating). Empirical proof: `BufferObjectLabellingTest_OpenGL_4_1.txt` contains only "Labelling Test" (labels unavailable → zero checks) while `_OpenGL_4_5.txt` records "Standard Top Level Checks: 2". Unforked, alternating platforms would toggle the committed file → perpetual churn.
- **Output/specificity forks the expected text.** Needed when message *content* is platform-dependent (driver-authored strings), even if the same checks run everywhere.
- Live examples of independence: `resourceful_context_free_test` = selectivity `opengl_version` + specificity `none` (checks vary with GL version; no platform-varying text laid down). `illegal_gpu_call_free_test` = selectivity `none` + `platform_specific` specificity (same checks everywhere; driver-flavoured text). Labelling tests (`GraphicsLabellingTestCore.hpp:24`) = selectivity `opengl_version` always, but specificity widens to `platform_specific` **in FP mode only** — FP mode records failure text, which is driver-flavoured.

**curlew implementation** ([[avocet-sequoia-usage]]): `selectivity_flavour` / `specificity_flavour` are deliberately *separate but identically-shaped* bitmask enums (`RenderingSetup.hpp:18-19` — os|renderer|opengl_version|build) so the axis is in the type and the two can't be cross-wired. Both feed `do_make_discriminator` (`RenderingSetup.cpp:66`) building ordered `OS_Renderer_OpenGL_M_m_Build` fragments from `rendering_setup` (queried once via `test_window_manager::find_rendering_setup()`). `platform_specific` = os|renderer|opengl_version; `full_specificity()` adds `build` (no current user of the build bit).

**Rule of thumb when adding a graphics test**: ask separately (a) does the *set of checks that execute* depend on OS/renderer/GL-version? → selectivity bits; (b) does any *laid-down text* (FP/FN output, caught exceptions) depend on them? → specificity bits. Default both to `none`; widen on demonstrated need (same philosophy as [[feedback-minimal-warning-suppression]] — specialization is a last resort).

**Gap noticed 2026-07-21**: nothing garbage-collects versioned output when a test is deleted. Automatic GC is on the user's sequoia TODO list (finding E4 in the 2026-07-21 review addendum). The 15 orphans then present (`InfiniteLoopFreeTest`, `VertexBufferObject*`) were manually removed the same day; a re-sweep (map each committed `output/TestSummaries` + `output/DiagnosticsOutput` filename back to a `Tests/**` source stem) is worth repeating after test deletions/renames.
