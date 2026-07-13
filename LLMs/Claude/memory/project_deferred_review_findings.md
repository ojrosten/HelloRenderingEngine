---
name: project-deferred-review-findings
description: Open findings from the 2026-07-10 full-codebase review — deferred by the user for lack of time; return to these in due course. Includes resolved-item log so nothing is re-raised.
metadata:
  type: project
---

On 2026-07-10 a full re-review of the codebase (Source, Tests, curlew, build system) was performed. The user triaged the findings; this file records what remains open so a future session can pick it up. Verify against current code before acting — items may have been fixed since.

## Resolved — do not re-raise

- **Typos/labels fixed** (commits `55a9dd83`, `083ec2ec`, 2026-07-10): `"one_minus_const_alpha "` trailing space; `"Frambuffer incomplete"`; `generic_resource` double-space message; `load_gl_fuctions`; duplicated `"Blend equation GPU/CPU"` check labels. The straggler `"MasK"` typo was fixed by the user 2026-07-13.
- **`string_view` → `glGetUniformLocation` NUL-termination hole** (`ShaderProgram.cpp:246`): deliberately kept — the user will make it *lecture content* later in the course, discussing the perennial question of whether C++ should support something like `zstring_view`. Don't propose a fix; it's planned material (same spirit as [[project-arithmetic-casts-cmp-migration]]). The eager uniform-map plan ([[project-shader-program-dsa-uniforms]]) will also remove this site.
- **`resourceful_context::do_utilize` discarding the LifeEvents instance** (static `LifeEvents::bind` call despite instance-detecting concepts, `ResourcefulContext.hpp:120-128`): will be sorted in lecture 50's unification. After L50 lands, verify and strike.
- **`get(ctx, string_names)` null check** (`Context/GLGetters.hpp:433`): **rejected by the user 2026-07-13** — a null return coincides with a raised GL error, so with the standard throwing epilogue clients are protected before the string is constructed; clients running without error checking have made an active choice to accept UB. (Note: libstdc++ throws `std::logic_error` on null-`const char*` string construction as QoI, but that's not standard and not portable to MSVC STL/libc++.) Don't re-raise.
- **curlew CMakeLists include-before-set** (`TestingUtilities/curlew/CMakeLists.txt:3`): reordered by the user 2026-07-13.
- **`run_with_lsan_suppressions.sh` path**: **not an issue** — the script is designed to be run from `build/TestAll`, from which `../../` correctly reaches the repo root. The review's CWD assumption (preset binary dir) was wrong. Don't re-raise.
- **`install_ubuntu_dependencies.sh` bootstrap**: fixed 2026-07-13 — `software-properties-common` now installed (guarded on `command -v add-apt-repository`) before the PPA step.

## Under consideration by the user

- **`get_info_log` doesn't trim the embedded NUL** (`Source/avocet/OpenGL/Resources/ShaderProgram.cpp:84-89`): `GL_INFO_LOG_LENGTH` includes the terminator, so compile/link exception messages carry `'\0'`. Siblings `Labels.cpp:39` and `Errors.cpp:37` trim. User wants to think more before fixing — ask, don't just patch.

## Open — production code

1. **`capable_context` is implicitly publicly movable** (`StateAwareContext/CapableContext.hpp`) while every other context layer protects its moves and all resources hold raw pointers to the context. Moving it with resources alive compiles, then terminates on resource destruction (throw through the hollowed glad context). curlew's immovable `window` shields this in practice. Question for the user: delete the leaf's moves, or document/enforce "context never relocates once resources exist"?
2. **`attrib_ptr_info::advance` vs `set_attribute_ptr` disagreement** (`Resources/Buffers.hpp:149-170`): `advance` implements the general multi-slot rule (right for dvec3/dvec4 today *and* mat4-sized attributes), but `set_attribute_ptr` emits one `VertexAttribPointer` capped at 4 components — a >4-component non-double attribute would get `GL_INVALID_VALUE`. Possibly staged generality; ask.

## Open — test infrastructure

4. **Serial-overlapping tracking-test teardown** (`Tests/OpenGL/Resources/ResourceTrackingUtilities.cpp:178-191`): destruction order `sp1 → win1 → sp0 → win0` means `sp0`'s GL deletes fire with no context current (win1's destruction detached it); quietly leaks win0's resource. Verdicts unaffected. Intentional "destruction under foreign context" customer, or accident?
5. **`make_call_logging_window_config` drops `ignored_warnings`** (`TestingUtilities/curlew/Window/WindowConfigurations.hpp:44`): hardcodes `default_debug_info_processor{}` unlike `make_default_config` (`GraphicsTestCore.hpp:80`). One NVIDIA driver update (message 131185) from unrelated red tests.
6. **Static-init GLFW bootstrap escapes `main`'s try/catch** (`GraphicsTestCore.hpp:26`, `inline static curlew::glfw_manager st_Manager{}`): failures during dynamic initialization (headless box, driver issue, the tripwire throws in `do_find_rendering_setup`) become message-less `std::terminate`, bypassing `TestAllMain.cpp`'s catch blocks. Lazy construction would restore the diagnostics.

## Open — build/infra

7. **`Demo/Examples/PonyPolygons.cpp:19` `get_dir`** throws on relative `std::source_location` paths, which the Ninja generator (Linux/Mac) produces — Demo effectively Windows-only. Fix options: `-fmacro-prefix-map`, or fallback via `fs::absolute`.
8. **Mac toolchain pin drift**: presets compile with unversioned Homebrew `llvm`, `Source/avocet/CMakeLists.txt` links `llvm@21`'s libc++ — skews when the formula bumps past 21.

## Observations (no action urged; context for future work)

- `framebuffer_object::bind(texture_unit)` binds the FBO and samples its own colour attachment in one call — feedback-loop territory if rendered in that state.
- `unique_image::operator==`: deep equality for vector-backed, pointer identity for stb-backed.
- `polygon`'s EBO capture relies on the VAO staying bound across the construction tail (member-order coupling).
- Capability-gated test suites reduce to zero checks on the Mac floor with no positive "skipped" assertion.
- Untested pure surfaces: `Version.hpp` predicates, `to_gl_underlying_value`, `to_array`, N≥5 polygons.
- shaderc/volk submodules unused (staged for Vulkan lectures?); README dependency list omits them; no CI anywhere; Windows builds `cxx_std_23` vs 26 elsewhere.
- Era-stale expected-output files `ShaderProgramBrokenStagesFreeTest_Exceptions_Linux_Mesa_OpenGL_4_5.txt` (pre-restructure path `Graphics/OpenGL/`, old message format) and `..._Linux_NVIDIA_OpenGL_4_6.txt` (old message format, line 184): user said **leave for now** (2026-07-13) — neither retire nor regenerate; don't re-raise as stale.

**How to apply:** when the user says "let's return to the review findings" (or similar), start here, re-verify line numbers against current code, and check the Resolved section before raising anything. Individual items may also resolve naturally via lectures (L50 unification, the zstring_view discussion) — prune as they do.
