---
name: avocet-sequoia-usage
description: "How HelloRenderingEngine/avocet consumes sequoia — production dependency, build system, curlew adaptation layer, value_tester specializations, committed output/"
metadata: 
  node_type: memory
  type: project
  originSessionId: f58cc83f-8059-4d66-a2ba-ed07ba4821c1
  modified: 2026-07-21T10:49:23.134Z
---

How this repo consumes sequoia (surveyed 2026-07). This is project-layer knowledge — sequoia's own memories ([[sequoia-overview]] and siblings) deliberately know nothing of avocet/curlew, mirroring the dependency structure.

**Sequoia is a production dependency, not test-only.** `Source/avocet` uses `mem_ordered_tuple`, `Maths/Geometry/Spaces.hpp` (`vec_coords` in `PolygonCoordinates.hpp` for `local_coordinates`/`texture_coordinates` — avocet's *only* direct Maths exposure, a small stable-looking alias that nonetheless lives in the churning `Spaces.hpp`, so sequoia updates can ripple into avocet through it), `to_array`, `Core/Meta/*` (ArithmeticCasts, CapableContext, ResourcefulContext, Image), `utilities::iterator` (`ContextualResource.hpp`), `FileSystem.hpp`, and `SEQUOIA_NO_UNIQUE_ADDRESS`. Sequoia's house style matches avocet's (`m_PascalCase`, brace-init, hidden friends, enum-classes over bools, deducing-this static dispatch = avocet's Pattern B).

**Build system is sequoia's**: every CMakeLists (`TestAll/`, `Demo/`, `Source/avocet/`, `TestingUtilities/curlew/`) includes `sequoia/build_system/Utilities.cmake` → `sequoia_init()` → `sequoia_finalize_{library,executable,tests}`. CMakePresets are 3-line includes of sequoia's `CMakePresetsCommon.json`. The sequoia target is added exactly once, in `Source/avocet/CMakeLists.txt` (`target_link_libraries(avocet PUBLIC sequoia)`); curlew/TestAll/Demo get it transitively. Chain: TestAll → curlew → {glfw, avocet}; avocet → {glad, sequoia, stb}. Build-system fixes belong to the sequoia project offline, not this repo.

**Registration** (`TestAll/TestAllMain.cpp` — in `TestAll/`, not repo root): ~44 test-header includes + 17 `add_test_suite` calls; each test appears ~3× (include, type, display string). Customizer: `{.source_folder{"avocet"}, .additional_dependency_analysis_paths{"TestingUtilities", "dependencies/sequoia/Source"}}` — so prune correctly invalidates tests when curlew *or sequoia itself* changes. `main` first queries `curlew::test_window_manager::find_rendering_setup()` and writes `Setup.txt` (full discriminator) beside the executable. The scaffolding CLI (`./TestAll create free ...`) machine-appends the boilerplate today. This mechanism is slated for retirement — [[sequoia-roadmap]].

**curlew is the adaptation layer** marrying sequoia's framework to GL realities (`TestingUtilities/curlew/`):
- `basic_graphics_test<Mode, Selectivity, Specificity, Extender>` derives from sequoia's `basic_test`; sets `parallelizable_type = std::false_type` (GL tests run serially) and implements `summary_discriminator()`/`output_discriminator()` from `rendering_setup_discriminator` so expected-output files fork per OS/renderer/GL-version/build only where needed (specialization is a last resort — baseline file plus suffixed variants like `..._Exceptions_Win_AMD_OpenGL_4_6.txt`). Forking is one of three layers keeping versioned exception files diff-stable — [[avocet-exception-message-stability]].
- `basic_graphics_labelling_test` capability-gates via deducing-this `run_tests(this Self& self)` — only calls `self.labelling_tests(w)` when `object_labels_available() == yes`.
- `selectivity_flavour`/`specificity_flavour` bitmask enums use sequoia's `as_bitmask` + `EXPOSE_SEQUOIA_BITMASK` (compile definition on curlew).
- Per-driver warning filters (`ignored_warnings`/`printed_then_ignored_warnings`) injected into default window config.

**Test taxonomy**: `free_test` (9 uses), `regular_test` (5 + FN diagnostics), `move_only_test` (`unique_image_test`, `resource_handle_test`), graphics variants layered by curlew, `resource_tracking_test<Resource>` in `Tests/OpenGL/Resources/ResourceTrackingUtilities.hpp` (lifetime tracking for shader programs/framebuffers). `check_semantics` is the workhorse in regular/move-only tests. One `transition_checker` user: `CapabilityManagerFreeTest.cpp`. FN-mode forces `if constexpr(Mode != test_mode::false_negative)` guards on always-passing infrastructure checks.

**value_tester specializations** (all in `Tests/**/​*TestingUtilities.hpp`): alignment, colour_channels, unique_image/image_view (shared `image_value_tester` base — the repo's canonical equality-vs-equivalence example: equality = spec + data-handle identity; equivalence = spec + content vs `image_data`), viewport family, a constrained template for empty capability types plus ~13 concrete capability testers, resource_handle, buffer objects (shared `generic_buffer_object_tester`), texture_2d.

**TestMaterials/ and output/**: `TestMaterials/` mirrors `Tests/` one dir per test class (PNGs, shaders, deliberately-broken files); sequoia copies to `output/TestsTemporaryData` working copies at runtime. `output/` is **committed** (~128 files): `DiagnosticsOutput/` (versioned FN/exception expected text with discriminator variants), `TestSummaries/`, `Prune/TestAll/<preset>/<config>/` (including `.external` listing out-of-tree includes like glad/stb).

**Friction is handled by policy, not wrappers**: no code-level workarounds for sequoia limitations exist in this repo; sequoia-side improvements are routed to the sequoia project offline (see [[sequoia-roadmap]] and the deferred-review-findings memory). Observed reliance: sequoia's exception-output comparison normalizes trailing whitespace. Cross-project parallel worth remembering: the reflection plans for test registration ([[sequoia-roadmap]]) and for avocet's `glad_ctx_member_info` table are the same C++26 bet.
