# Open items — consolidated index

**Last updated:** 2026-07-19 (post coverage analysis).
**What this is:** the single, version-controlled index of every open actionable item for the project, consolidated across the review documents and memory files. Detail lives in the linked sources; this file is the map. **Maintenance contract:** any session that resolves, re-scopes, or adds an item should update this file (and the linked source) in the same breath.

Detailed sources:
- `reviews/coverage_analysis_2026-07-18.md` — coverage report analysis (avocet/curlew/Tests), false-positive taxonomy, false-negative sweep
- `reviews/sequoia_review_2026-07-16.md` — sequoia full review (separate project; findings tracked there)
- `memory/project_deferred_review_findings.md` — 2026-07-10 full-codebase review: open/parked/resolved log
- `memory/project_coverage_analysis_2026_07.md`, `memory/project_coverage_report_generation.md` — coverage conclusions & generation recipe
- `memory/project_planned_lecture_topics.md`, `memory/project_graph_based_testing_opportunities.md`, `memory/project_vulkan_branch_merges.md`

---

## 1. Ready now (small, no trigger needed)

| Item | Detail | Source |
|---|---|---|
| **Validation-throw tests** | One-line `check_exception_thrown`s for three reachable guards: `Image.cpp:38` (>4 channels), `Textures.hpp:86` (alignment >8), `GLFWWrappers.hpp:32` (`num_samples{0}`). Agreed worth addressing 2026-07-19 ("we'll come back to this"). | coverage review, item 2 |
| **`Polygon.hpp` invisible surface** | Instantiate-and-exercise: 0/1-texture ctor overloads, 0-texture & single-`texture_unit` `draw`, `operator==`, moves (polygons are on the resources-keep-their-moves side). N ≥ 5 is *already ruled out* (2026-07-14 — triangles+quads cover both equivalence classes); dimensionality-4 an open question. | coverage review, item 4 |
| **`GLGetters.hpp` five getter families** | `get` over `int64/paired_int/paired_float/quadruple_bool/quadruple_int` names: never instantiated, zero customers anywhere. Decide groundwork-vs-test. | coverage review, item 6 |
| **Upstream coverage-pipeline fixes** | `-fprofile-update=atomic` (base-coverage preset), `-T Coverage` dropped from ctest line, `inconsistent,inconsistent` dedup at `lcov --remove` — verified working, still uncommitted in the WSL clone's `dependencies/sequoia`. Belongs in the sequoia repo. | `memory/project_coverage_report_generation.md` |
| *(Optional)* **Mac coverage run** | Union with the Linux report verifies the platform-gated false positives empirically; isolates true hardware-only residue. | coverage review, "What the Mac union would do" |

## 2. Parked — explicit triggers

| Item | Trigger | Status |
|---|---|---|
| **Delete moves across the context hierarchy** (`unique_glad_context`/`context_base` `= delete`; remove `= default` re-enablements up through `resourceful_context`; `capable_context` follows) + `unique_ptr<window>` | Test-efficiency / parallel-graphics-tests work | **Near-settled** (2026-07-19: coverage sweep proved no context move was ever instantiated; user: "almost certainly want to delete"). Implement, don't reopen analysis. |
| **Capability-gated suites: positive "skipped" assertions** (3 boolean checks in `create_window` pass-through; `if constexpr(Mode != false_negative)`) | Same trigger — falls out of `create_window` returning `unique_ptr<window>` | Parked 2026-07-14; design points recorded in deferred-findings memory |
| **CI roadmap** (Actions compile matrix; Linux xvfb+llvmpipe run vs existing expected outputs; Win/Mac compile-only) | User returns to it | Parked at user request 2026-07-14; sketch in deferred-findings memory |
| **Call-logging config harmonization** with `make_default_config` (probable shape: `const rendering_setup&` param) | `make_call_logging_window_config` grows more/varied customers | Rejected for today's two call sites, expected on growth |

## 3. Lecture-bound — resolve as course content

- **DSA `glProgramUniform*` migration** — also: fixes `set_uniform` deducing-this exception, narrows program-binding cache, eager uniform map, removes the `string_view`/NUL site. (`memory/project_shader_program_dsa_uniforms.md`)
- **`attrib_ptr_info::advance` vs `set_attribute_ptr` cap** — geometrical-transformations lectures ("soonish").
- **`framebuffer_object::bind` → `bind_as`** (draw/read split; no self-sampling bind; per-target cache slot) — multi-pass rendering. The paired draw/read checks at the end of `FramebufferTrackingFreeTest` are deliberate instrumentation for this split.
- **Polygon EBO↔VAO coupling made syntactic** (EBO ctor takes VAO; DSA `glVertexArrayElementBuffer` path) — polygon-revisit lecture.
- **`checked_conversion_to` constraint over-promise** — lecture 51 homework; user supplies the fix. Retire `memory/project_arithmetic_casts_cmp_migration.md` when it lands.
- **Incomplete-FBO error strings** (`Framebuffer.cpp` 33.3% coverage) — deferred by design; the bare-minimum FBO implementation will evolve.
- **Roadmap topics** (see `memory/project_planned_lecture_topics.md`): strongly-typed texture parameters; buffer-storage policies; render-graph/draw-queue; UBO abstraction; FBO depth/stencil; shader-label rejig (defaults move to curlew, monadic `std::optional`); C++26 reflection over `GladGLContext` (eventual).

## 4. Standing asks & side-projects

- **Graph-based-testing opportunities** (user's standing request, 2026-07-17): shader-program uniform state (most valuable *before* the DSA migration), binding-cache × resource-lifetime serial scenarios, buffer/texture state (weaker). (`memory/project_graph_based_testing_opportunities.md`)
- **`to_array` migration to sequoia** — gains tests there; don't test/flag it here meanwhile.
- **Vulkan**: periodic lecture-branch merges into `vulkan-experiment` (recipe in `memory/project_vulkan_branch_merges.md`); Demo run is the acceptance test.
- **Windows ASan DLL workflow fix** — sequoia build system, handled offline by the user.

## 5. Explicitly closed by decision (don't re-raise)

- `to_string(gl_capability)` partial coverage — **kept as a signal**: the covered subset implicitly tracks how many GL capabilities are in use; a round-trip test would destroy it (2026-07-19).
- N ≥ 5 polygon tests; pentagon suggestions (2026-07-14).
- The larger resolved/rejected log lives in `memory/project_deferred_review_findings.md` ("Resolved — do not re-raise").
