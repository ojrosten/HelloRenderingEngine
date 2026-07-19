# Coverage report analysis: avocet & curlew

**Date:** 2026-07-18
**Analyzed state:** `start_lecture_51`, lcov report from `linux-pkg-gcc-coverage` (g++-15, WSL Ubuntu, llvmpipe, OpenGL 4.5, Debug), regenerated after the atomic-counter fix (see below) — counts stable across consecutive runs.
**Method:** per-file rates via `lcov --list` on the filtered tracefile; uncovered lines extracted from `DA:` records and zero-hit function aliases from `FNL:`/`FNA:` records; every flagged site read in source and classified. Third-party dependencies (sequoia, stb, glad) excluded per scope. Calibration applied: defensive throws are intentional tripwires; Tests/ (and Demo/) are real customers.

**Verdict up front.** avocet's real line coverage is **90.5%** and the uncovered remainder decomposes almost entirely into five explainable categories, only one of which is actionable. There are no alarming holes; the "worst" files (`Errors.cpp` 41.1%, `Framebuffer.cpp` 33.3%, `Formatting.hpp` 41.4% functions) are respectively platform-gating, a small genuine gap, and the tripwire policy made visible. The most useful outputs are (a) a short list of reachable-but-untested validation throws and (b) a set of systematic cautions about how lcov's numbers mislead in both directions.

## Headline recalibration

The report's global 48.9% is dominated by dependencies (only `/usr/*` is filtered out). Restricted by layer:

| Layer | Line coverage |
|---|---|
| `Source/avocet` | **90.5%** (1148/1269) |
| `TestingUtilities/curlew` | **83.8%** (160/191) |
| `Tests/` + `TestAll` | 96.5% (1427/1478) |

At 100% (lines): all of `Core/Geometry`, `Core/Utilities`, `OpenGL/Geometry`, `OpenGL/Utilities`, `ResourceInfrastructure` headers, `StateAwareContext` (lines), and most of `Resources`.

## False negatives — uncovered code that isn't a testing gap

### 1. Platform-gated: the Mac (GL 4.1 / libc++) run fills these

- **`Errors.cpp` (41.1% lines — worst avocet file, almost entirely this category):**
  - Lines 64–87: `libcpp_workaround::{get_errors, get_messages}` — dead on gcc (has `std::generator`); the `#ifdef __cpp_lib_generator` selects the generator versions. Only libc++ executes the workaround.
  - Lines 99–117: the whole `to_string(error_code)` switch. On GL 4.5 error handling flows through the debug-message path (`GetDebugMessageLog`); on 4.1 the `glGetError` path runs and the illegal-GPU-call tests would hit `invalid_enum`/`invalid_value`/`invalid_operation`. (`stack_overflow`/`stack_underflow`/`out_of_memory` cases are unreachable-in-practice on any platform.)
  - Line 30: `get_next_message`'s nullopt when `max_debug_message_length()` is empty (no debug context).
- **Object-labels-unavailable branches** (labelling == `no` only on 4.1): `CharacteristicContext.cpp:16`, `Labels.cpp:26` (`return ""`), `ShaderProgram.cpp:43/51` (`return std::nullopt` in `make_program_label`/`make_stage_label`).
- **curlew OS branches:** `GLFWWrappers.cpp:111-112` (Win/Apple expected-version check; the throw itself is a tripwire), `RenderingSetup.cpp:46/48` (`"Apple"` / fallback in `operating_system()`).
- Partial fills: `Errors.cpp:126-131` — only `debug_severity::high` is stringified on llvmpipe; Mac's glGetError path doesn't help here, but other severities may appear on real drivers (overlaps category 2).

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

### 4. Scope false negatives — the customer isn't under the harness

`window_resource::get_framebuffer_extent` (`GLFWWrappers.cpp:137-142`) and `window::update_viewport` (`:157-159`) are consumed by `Demo/DemoMain.cpp:58-59`; the demo doesn't run under TestAll coverage. Error-path plumbing in the same family: `errorCallback` (22–24), glfwInit/GLAD failure throws (65, 54) — only fire when GLFW/GLAD actually fail.

### 5. gcov attribution artifacts — noise, ignore

Zero-count lines inside demonstrably-executing straight-line code: `GLFWWrappers.cpp:102` (return between hit lines), `Framebuffer.hpp:56` (ctor init-list line), `Errors.cpp:176-177` (format-argument lines), and closing braces at `Buffers.hpp:194` / `RenderingSetup.cpp:83` — never-taken exception-cleanup blocks that `geninfo_unexecuted_blocks=1` deliberately surfaces on `}` lines. In Tests/, the always-throwing `check_exception_thrown` lambda bodies (e.g. `ArithmeticCastsFreeTest.cpp` at 71.4%) are the same artifact family — the exceptional edge isn't instrumented.

## False positives — where "covered" overstates what's tested

- **A hit line is not a taken branch.** `if(is_amd(renderer)) return "AMD";` counts as covered when the condition merely evaluated false — only the llvmpipe row of `RenderingSetup.cpp`'s renderer table has actually *returned*. Sharper instrument: lcov branch coverage (not currently collected; would slow capture and add exception-edge noise).
- **Function coverage counts aliases** (lcov 2.x `FNL`/`FNA`: each template instantiation / lambda-context is an alias of a line-range leader). Distorts both directions: `GenericResource.hpp` "100% of 499 functions" mostly means "each instantiation entered once"; `CapableContext.hpp`'s 83% is *deflated* because the `null_function_pointer_free_test` context instantiation aborts by design before its per-capability lambdas run, leaving ~9 never-hit aliases whose identical siblings are exercised elsewhere.
- **Fixture traffic.** Context/capability/window code executes in every test via the curlew fixture; e.g. `formatter<gl_capability>` has 342 hits, mostly from producing test output. Execution ≠ assertion — though dedicated tests (CapabilityManager, Version, ResourcefulContext) do assert on this machinery.
- **Debug-only view.** Every `has_ndebug()` branch is systematically dark in this report.

## Actionable shortlist (small, llvmpipe-reachable; decide deliberately)

1. **`Framebuffer.cpp` 33.3%** — incomplete-FBO error strings (15–30). A deliberately incomplete FBO (no attachments → `INCOMPLETE_MISSING_ATTACHMENT`) covers the mechanism; most of the eight cases stay hardware-dependent.
2. **Reachable validation throws with no exercising test** (unlike tripwires, these guard constructible inputs): `Image.cpp:38` (>4 requested channels), `Textures.hpp:86` (`to_ogl_alignment` with alignment >8), `GLFWWrappers.hpp:32` (`num_samples{0}`). Each is a one-line `check_exception_thrown`.
3. **`Capabilities.cpp` 68.9%** — `to_string(gl_capability)` covers only the nine capabilities tests happen to stringify; a round-trip test over all thirty enumerators would pin the name table (typos currently only surface in error messages).

## What the Mac union would and wouldn't do

Running the same report on Mac (GL 4.1, libc++) and unioning: category 1 moves from "explained" to "verified" — notably `Errors.cpp` should jump dramatically (workaround bodies + glGetError path + labels-unavailable branches). Categories 2 (hardware residue), 3 (tripwires), and the Demo-scope items are untouched by any platform union; that residue is irreducible in CI and healthy.

## Appendix: generation-pipeline fixes (2026-07-18, precede this analysis)

Report determinism was fixed first (details in memory `project_coverage_report_generation.md`): `-fprofile-update=atomic` in the base-coverage preset (cured an lcov `negative` hit-count error and cross-file jitter from torn non-atomic counters under TBB threads), `-T Coverage` dropped from the script's ctest line, doubled `inconsistent,inconsistent` suppression at `lcov --remove` (genhtml stays the single loud reporter). Sequoia-side changes uncommitted at time of writing. Residual honest nondeterminism: `sequoia TestFramework/Summary.cpp` duration-formatting branches (wall-clock dependent). The 8 remaining genhtml `inconsistent` warnings are the always-throwing / never-invoked lambda artifacts and are expected.
