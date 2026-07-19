# Coverage report analysis: avocet & curlew

**Date:** 2026-07-18 (terminology and Polygon.hpp findings revised 2026-07-19; systematic false-negative sweep and Tests/ analysis added 2026-07-19)
**Analyzed state:** `start_lecture_51`, lcov report from `linux-pkg-gcc-coverage` (g++-15, WSL Ubuntu, llvmpipe, OpenGL 4.5, Debug), regenerated after the atomic-counter fix (see appendix) — counts stable across consecutive runs.
**Method:** per-file rates via `lcov --list` on the filtered tracefile; uncovered lines extracted from `DA:` records and zero-hit function aliases from `FNL:`/`FNA:` records; every flagged site read in source and classified; for Polygon.hpp, the emitted-function line ranges compared against the full template surface. Third-party dependencies (sequoia, stb, glad) excluded per scope. Calibration applied: defensive throws are intentional tripwires; Tests/ (and Demo/) are real customers.

**Terminology.** The report is a diagnostic test whose condition is "gap in coverage". A **positive** is a flagged gap (uncovered line/function). Hence a **false positive** is flagged code that isn't a real gap, and a **false negative** is a real gap the report fails to flag — including code the report cannot see at all.

**Verdict up front.** avocet's real line coverage is **90.5%** and the flagged remainder decomposes almost entirely into five false-positive categories; the "worst-looking" files (`Errors.cpp` 41.1%, `Formatting.hpp` 41.4% functions) are respectively platform-gating and the tripwire policy made visible. The more consequential findings are on the **false-negative** side: never-instantiated templates emit no gcov records, so `Polygon.hpp` reports 100% while both ergonomic ctor/draw overload families, `operator==`, and move operations don't exist in the binary — addressable with improved test coverage.

## Headline recalibration

The report's global 48.9% is dominated by dependencies (only `/usr/*` is filtered out). Restricted by layer:

| Layer | Line coverage |
|---|---|
| `Source/avocet` | **90.5%** (1148/1269) |
| `TestingUtilities/curlew` | **83.8%** (160/191) |
| `Tests/` + `TestAll` | 96.5% (1427/1478) |

At 100% (lines): all of `Core/Geometry`, `Core/Utilities`, `OpenGL/Geometry`, `OpenGL/Utilities`, `ResourceInfrastructure` headers, `StateAwareContext` (lines), and most of `Resources`.

## False positives — flagged code that isn't a real gap

### 1. Platform-gated: the Mac (GL 4.1 / libc++) run fills these

- **`Errors.cpp` (41.1% lines — worst avocet file, almost entirely this category):**
  - Lines 64–87: `libcpp_workaround::{get_errors, get_messages}` — dead on gcc (has `std::generator`); the `#ifdef __cpp_lib_generator` selects the generator versions. Only libc++ executes the workaround.
  - Lines 99–117: the whole `to_string(error_code)` switch. On GL 4.5 error handling flows through the debug-message path (`GetDebugMessageLog`); on 4.1 the `glGetError` path runs and the illegal-GPU-call tests would hit `invalid_enum`/`invalid_value`/`invalid_operation`. (`stack_overflow`/`stack_underflow`/`out_of_memory` cases are unreachable-in-practice on any platform.)
  - Line 30: `get_next_message`'s nullopt when `max_debug_message_length()` is empty (no debug context).
- **Object-labels-unavailable branches** (labelling == `no` only on 4.1): `CharacteristicContext.cpp:16`, `Labels.cpp:26` (`return ""`), `ShaderProgram.cpp:43/51` (`return std::nullopt` in `make_program_label`/`make_stage_label`).
- **curlew OS branches:** `GLFWWrappers.cpp:111-112` (Win/Apple expected-version check; the throw itself is a tripwire), `RenderingSetup.cpp:46/48` (`"Apple"` / fallback in `operating_system()`).
- Partial fills: `Errors.cpp:126-131` — only `debug_severity::high` is stringified on llvmpipe; other severities may appear on real drivers (overlaps category 2).

### 2. Hardware/driver-gated: neither Linux-llvmpipe nor Mac fills these (permanent CI residue)

- `CapabilitiesConfiguration.cpp:109-110` — the **Intel Arc** `StencilFunc` init-bug compensation. This single call site also explains the unhit `gl_function<void(GLuint, GLint, GLuint)>` instantiation (ctor, `operator()`, `get_name`, `get_validated_fn_ptr` aliases in `GLFunction.hpp`) and the matching `decorated_context::invoke` alias — glStencilFunc is instantiated *only* here.
- `GLFWWrappers.cpp:79` (Intel `message_id{2}`), `:83/93` (NVIDIA `131218`/`131185`).
- `Errors.hpp:123` — `std::println` of notification / print-then-ignore messages; llvmpipe emits no notifications.
- `RenderingSetup.cpp:57/58/60` — mesa/NVIDIA renderer rows and empty-fallback in `concise_renderer` (llvmpipe matches first); `:99` — a renderer-predicate body never invoked for the same reason.
- `GLFWWrappers.cpp:120` — the some-Linux-systems 4.1→4.6 retry quirk.
- `to_string(debug_source/debug_type)` cases beyond `api`/`shader_compiler`/`error` (`Errors.cpp:141-148, 157-167`) — llvmpipe only emits those.

### 3. Intentional tripwires — correctly uncovered, by policy

All post-exhaustive-switch `throw std::runtime_error{error_message(...)}` sites and kin: 6 in `Capabilities.cpp` (49, 72, 86, 97, 114, 131, 142), `ShaderProgram.cpp:35`, `ContextBase.cpp:34` and `:86` (the `init_debug` inconsistency throw), `Textures.hpp:53/67/80`, `GenericResource.hpp:209` (null resource), `GLFunction.hpp:73` (`out_of_range`), curlew `GraphicsTestCore.hpp:34-37` (catch + terminate around static GLFW init).

**This category fully explains `Formatting.hpp`'s 41.4% function coverage**: `error_message<E>` is instantiated for ~12 production enums but is *reachable only from tripwires*, so only the test-local `whatever` enum's instantiation is ever called. Ditto `make_error_message<unsigned char/unsigned int>`, `to_string(debugging_mode)` (`ContextBase.cpp:26-33`), and `formatter<debugging_mode>`. Not a gap — the tripwire policy made visible in coverage data.

### 4. Scope: the customer isn't under the harness

`window_resource::get_framebuffer_extent` (`GLFWWrappers.cpp:137-142`) and `window::update_viewport` (`:157-159`) are consumed by `Demo/DemoMain.cpp:58-59`; the demo doesn't run under TestAll coverage. Error-path plumbing in the same family: `errorCallback` (22–24), glfwInit/GLAD failure throws (65, 54) — only fire when GLFW/GLAD actually fail.

### 5. gcov attribution artifacts — noise, ignore

Zero-count lines inside demonstrably-executing straight-line code: `GLFWWrappers.cpp:102` (return between hit lines), `Framebuffer.hpp:56` (ctor init-list line), `Errors.cpp:176-177` (format-argument lines), and closing braces at `Buffers.hpp:194` / `RenderingSetup.cpp:83` — never-taken exception-cleanup blocks that `geninfo_unexecuted_blocks=1` deliberately surfaces on `}` lines. In Tests/, the always-throwing `check_exception_thrown` lambda bodies (e.g. `ArithmeticCastsFreeTest.cpp` at 71.4%) are the same artifact family — the exceptional edge isn't instrumented.

## False negatives — real gaps the report doesn't flag

- **Never-instantiated templates are invisible, not merely uncounted.** gcov only sees code the compiler emitted; an uninstantiated template contributes nothing to the denominator. **`Polygon.hpp` is the flagship case: reported 100% (lines and functions), yet** — established by comparing emitted `FNL` line ranges against the template surface — the following have never existed in the binary:
  - the `num_textures == 0` and `== 1` constructor overloads (`polygon_base` 38/46, `polygon` 114/122) — every test goes through the span-based ctor (55/130);
  - the no-texture `draw` (64–66) and single-`texture_unit` `draw` (70–73) — only the span overload (76–81) is instantiated;
  - `operator==` (84, defaulted friend) — polygon equality never once emitted;
  - the protected move ctor/assignment (88–89) — polygons never moved;
  - instantiation axes: only N ∈ {3, 4} and dimensionality {2, 3}, though the constraints admit N ≤ 87 and dimensionality 4.
  (`to_element_index` (145–151) is also absent but structurally so — consumed at compile time by the constexpr `st_Indices`.)
  **Addressable with improved test coverage**; per-instantiation, the same blindness applies to any template-heavy header, so "100%" on such files should always be read as "100% of what was instantiated".
- **Systematic sweep for more of the same (2026-07-19)** — method: per file, code-like lines with *no `DA:` record at all* (as opposed to `DA:…,0`), plus repo files with no `SF:` record; every hit read and classified. Two further genuine finds:
  - **`GLGetters.hpp` (reported 100% of 22 functions): five entire getter families never instantiated** — `get(…, int64_names)` (391), `get(…, paired_int_names)` (403), `get(…, paired_float_names)` (409), `get(…, quadruple_bool_names)` (415), `get(…, quadruple_int_names)` (421). Grep confirms their name-enums (`timestamp`, `max_server_wait_timeout`, `scissor_box`, `viewport`, `color_writemask`, `depth_range`, …) have **zero consumers anywhere** — production, Tests, or Demo. Spec-transcribed groundwork with no customer yet; a fixture test querying each would both instantiate and pin them, or leave deliberately as groundwork.
  - **`unique_glad_context` move constructor and move assignment never instantiated** (`ContextBase.hpp:38-45`). Unlike Polygon's these are *hand-written* bodies (`std::exchange`; swap-based assignment) — the class's raison d'être is movable GladGLContext ownership, and that logic has never executed. CPU-only, cheaply testable.
  - Interpretive notes from the sweep: (a) *non-trivial defaulted* members do emit records when odr-used (`~polygon_base()` = default appears with 32 aliases), so the absence of Polygon's and `unique_glad_context`'s moves is meaningful, not a defaulted-function blind spot; (b) the `std::generator` versions of `get_errors`/`get_messages` **are** called on Linux (hits 91 / 4632) — their body lines lacking DA records is coroutine code-splitting attribution, correcting any assumption that the generator path is Mac-only dead weight; (c) five headers have no `SF:` record at all (`ObjectIdentifiers.hpp`, `Messages.hpp`, `TypeTraits.hpp`, `CapabilitiesConfiguration.hpp`, `Labels.hpp`) — all declaration-only or compile-time traits, structurally invisible and fine; (d) everything else the sweep surfaced is structural: enums, concepts/traits, member declarations, designated-initializer continuation lines, the constexpr `glad_ctx_member_info` table.
- **A hit line is not a taken branch.** `if(is_amd(renderer)) return "AMD";` counts as covered when the condition merely evaluated false — only the llvmpipe row of `RenderingSetup.cpp`'s renderer table has actually *returned*. Sharper instrument: lcov branch coverage (not currently collected; would slow capture and add exception-edge noise).
- **Function coverage counts aliases** (lcov 2.x `FNL`/`FNA`: each template instantiation / lambda-context is an alias of a line-range leader). Distorts both directions: `GenericResource.hpp` "100% of 499 functions" mostly means "each instantiation entered once"; conversely `CapableContext.hpp`'s 83% overstates the gap — the `null_function_pointer_free_test` context instantiation aborts by design before its per-capability lambdas run, leaving ~9 never-hit aliases whose identical siblings are exercised elsewhere.
- **Fixture traffic.** Context/capability/window code executes in every test via the curlew fixture; e.g. `formatter<gl_capability>` has 342 hits, mostly from producing test output. Execution ≠ assertion — though dedicated tests (CapabilityManager, Version, ResourcefulContext) do assert on this machinery.
- **Debug-only view.** Every `has_ndebug()` branch is systematically dark in this report.

## Tests/ analysis (added 2026-07-19)

96.5% lines (1427/1478); no genuine gaps in test code. The flagged remainder decomposes into three families, all benign:

1. **Always-throwing `check_exception_thrown` lambdas** (the dominant family; exceptional edge uninstrumented, so bodies show `DA:0` despite executing): all of `ArithmeticCastsFreeTest.cpp`'s 71.4% (5 lambdas), `MessagesFreeTest.cpp` 85.2% (2), `ImageViewTest.cpp`, `UniqueImageTest.cpp`, `ShaderProgramFileExistenceFreeTest.cpp` (4), `ShaderProgramBrokenStagesFreeTest.cpp` (3), `NullFunctionPointerFreeTest.cpp`, `MultipleIllegalGPUCallsFreeTest.cpp`.
2. **Aggregate-init / continuation-line attribution**: `CapabilityManagerFreeTest.cpp:339-352` (the transition-graph `make_payload` node list — the payloads demonstrably exist), `PolygonFreeTest.cpp:147`, `FramebufferFreeTest.cpp:41/61`, `ImageTestingUtilities.cpp:32`.
3. **Failure branches of passing tests — dark by construction when the suite is green**: `UniqueImageThreadingFreeTest.cpp:73` (`return false;` on image mismatch), `TestAllMain.cpp:184-191` (top-level catch blocks). Their coverage would signal test *failure*; uncovered is the healthy state.

Invisible side: the 46 Tests/*.hpp files with no `SF:` record are declaration-only headers — legitimate. `PolygonFreeTest`'s templated drivers (`poly_data`, `test_polys`, `test_poly`) mirror the Polygon.hpp instantiation axes from the consumer side — same finding, same fix. `ResourceTrackingUtilities.cpp` explicitly instantiates `resource_tracking_test` for `shader_program` and `framebuffer_object` only (230-231) — a statement of current scope rather than a gap.

## Actionable items — dispositions agreed 2026-07-19

1. ~~Incomplete-FBO error strings (`Framebuffer.cpp` 33.3%)~~ — **deferred by design**: the bare-minimum FBO implementation is expected to evolve; the gap will change shape naturally. Don't chase now.
2. **Reachable validation throws with no exercising test** — agreed worth addressing (pending): `Image.cpp:38` (>4 requested channels), `Textures.hpp:86` (`to_ogl_alignment` with alignment >8), `GLFWWrappers.hpp:32` (`num_samples{0}`). Each is a one-line `check_exception_thrown`. Unlike tripwires, these guard constructible inputs.
3. ~~`to_string(gl_capability)` at 68.9%~~ — **reframed, keep as-is**: the covered subset implicitly tracks roughly how many OpenGL capabilities the project actually uses. It's a signal, not a gap; a round-trip test over all thirty enumerators would destroy the signal.
4. **`Polygon.hpp` uninstantiated surface** (new, from the false-negative analysis): instantiate-and-exercise the 0/1-texture ctor and draw overloads, `operator==`, and move operations; consider whether N ≥ 5 and dimensionality-4 instantiations deserve a test. Addressable with improved test coverage.
5. **`unique_glad_context` moves** (from the 2026-07-19 sweep): hand-written move ctor/assignment never executed — and the correct resolution is **deletion, not testing**. This finding is binary-level confirmation of the parked 2026-07-13 analysis (see `project_deferred_review_findings.md` item 1): delete moves across the whole context hierarchy (`unique_glad_context`/`context_base` `= delete`, remove the protected `= default` re-enablements up through `resourceful_context`; `capable_context` follows for free). Testing these moves would exercise behavior slated to be outlawed; deletion resolves the coverage false negative by removal. Distinct from the Polygon move gap, which sits on the resources-keep-their-moves side and resolves by exercising (item 4).
6. **`GLGetters.hpp` five uninstantiated getter families** (from the sweep): decide groundwork-vs-test — zero customers anywhere today; a fixture test querying each name would pin the spec transcription, or the absence can stand as deliberate groundwork for future lectures.

## What the Mac union would and wouldn't do

Running the same report on Mac (GL 4.1, libc++) and unioning: false-positive category 1 moves from "explained" to "verified" — notably `Errors.cpp` should jump dramatically (workaround bodies + glGetError path + labels-unavailable branches). Categories 2 (hardware residue), 3 (tripwires), the Demo-scope items, and all the false negatives are untouched by any platform union; that residue is irreducible in CI and (categories 2–4) healthy.

## Appendix: generation-pipeline fixes (2026-07-18, precede this analysis)

Report determinism was fixed first (details in memory `project_coverage_report_generation.md`): `-fprofile-update=atomic` in the base-coverage preset (cured an lcov `negative` hit-count error and cross-file jitter from torn non-atomic counters under TBB threads), `-T Coverage` dropped from the script's ctest line, doubled `inconsistent,inconsistent` suppression at `lcov --remove` (genhtml stays the single loud reporter). Sequoia-side changes uncommitted at time of writing. Residual honest nondeterminism: `sequoia TestFramework/Summary.cpp` duration-formatting branches (wall-clock dependent). The 8 remaining genhtml `inconsistent` warnings are the always-throwing / never-invoked lambda artifacts and are expected.
