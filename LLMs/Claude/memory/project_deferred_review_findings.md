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
- **`run_with_lsan_suppressions.sh` path**: initially rejected ("designed to run from `build/TestAll`"), but on actually exercising it the user found the review was right — fixed to `../../../` (commit `6681a091`, 2026-07-13), matching the preset binary dir `build/TestAll/<presetName>/`.
- **`install_ubuntu_dependencies.sh` bootstrap**: fixed 2026-07-13 — `software-properties-common` now installed (guarded on `command -v add-apt-repository`) before the PPA step.

## Under consideration by the user

- **`get_info_log` NUL trim: resolved 2026-07-13** (commits `d3646f8d`, `0488d270`): the user harmonized all three trimming sites through `avocet::opengl::trim(std::string&, GLsizei)` (`Source/avocet/OpenGL/Utilities/Messages.{hpp,cpp}`) — one conditional handles both GL length conventions; a `logic_error` tripwire guards length > buffer; `MessagesFreeTest` covers both conventions, boundaries, and both throws. Verified across the full platform matrix (Apple/NVIDIA runs only rippled the earlier typo/label fixes into the FN diagnostic files). The trailing-`\n` watch item **did not fire** — sequoia's exception-output comparison evidently normalizes trailing whitespace (useful to remember when predicting whether message-format changes will invalidate versioned outputs).

## Open — production code

1. **`capable_context` is implicitly publicly movable** (`StateAwareContext/CapableContext.hpp`) while all resources hold raw pointers to the context via `context_ref`. Discussed in depth with the user 2026-07-13:
   - `context_ref`/`resource_handle`'s swap-based move-assignment (and exchange-to-null move-construction) make *resource* moves fully safe — (context, handle) pairs always travel coherently, moved-from resources destroy handle 0 benignly. This is deliberate, elegant design; don't re-litigate.
   - But nothing tracks the *context object* relocating: move-construction hollows the source (`unique_glad_context` exchange, `ContextBase.hpp:38`) → resource dtors throw through null fn pointers → terminate; move-**assignment** *swaps* dispatch tables between two live contexts → **silent** cross-context corruption (the worse path — no tripwire).
   - The user floated making `window`s movable (parallel graphics tests: GLFW needs main-thread window creation, so create windows at test construction and move them with the tests) — but tests holding `std::unique_ptr<window>` achieves that with address-stable windows, no `GLFWwindow&`-member surgery, no user-pointer audits, no invariant policing.
   - **Leading resolution (2026-07-13, user leaning, not yet done): delete moves from the entire context hierarchy** — `= delete` at `context_base`, then *remove* the protected `= default` re-enablements in `context`/`decorated_context`/`characteristic_context`/`resourceful_context` (don't leave them: they'd silently become defined-as-deleted). Verified safe today: `create_window` returns a prvalue (guaranteed RVO, `GLFWWrappers.cpp:126`), nothing in the repo moves a context object, nothing stores windows in containers. Resources KEEP their moves.
   - Loss analysis (done, lopsided): every casualty of immovability — by-value windows in movable aggregates, in-place window reassignment, named-local factory returns — has a one-line `unique_ptr` recovery; movable windows would instead carry permanent invariant-policing debt plus the silent move-assign corruption path. Entity semantics: a GL context names driver-side state with identity; deleted moves are type-as-documentation (house ethos). Lecture-sized topic.
2. **`attrib_ptr_info::advance` vs `set_attribute_ptr` disagreement** (`Resources/Buffers.hpp:149-170`): `advance` implements the general multi-slot rule (right for dvec3/dvec4 today *and* mat4-sized attributes), but `set_attribute_ptr` emits one `VertexAttribPointer` capped at 4 components — a >4-component non-double attribute would get `GL_INVALID_VALUE`. Possibly staged generality; ask.

## Open — test infrastructure

4. **Serial-overlapping tracking-test teardown** (`Tests/OpenGL/Resources/ResourceTrackingUtilities.cpp:178-191`): destruction order `sp1 → win1 → sp0 → win0` means `sp0`'s GL deletes fire with no context current (win1's destruction detached it); quietly leaks win0's resource. Verdicts unaffected. Intentional "destruction under foreign context" customer, or accident?
5. **`make_call_logging_window_config` drops `ignored_warnings`** (`TestingUtilities/curlew/Window/WindowConfigurations.hpp:44`): hardcodes `default_debug_info_processor{}` unlike `make_default_config` (`GraphicsTestCore.hpp:80`). One NVIDIA driver update (message 131185) from unrelated red tests.
6. **Static-init GLFW bootstrap escapes `main`'s try/catch** (`GraphicsTestCore.hpp:26`, `inline static curlew::glfw_manager st_Manager{}`): failures during dynamic initialization (headless box, driver issue, the tripwire throws in `do_find_rendering_setup`) become message-less `std::terminate`, bypassing `TestAllMain.cpp`'s catch blocks. Lazy construction would restore the diagnostics.

## Open — build/infra

7. ~~Demo Windows-only via `get_dir`~~ **Not an issue — refuted empirically 2026-07-13**: the user confirmed the Demo runs on Linux and Mac as well as Windows. The finding's premise (CMake's Ninja generator passing *relative* source paths, making `std::source_location::file_name()` relative) doesn't hold on the toolchains in use — modern CMake evidently passes absolute paths. The `"Relative paths not supported"` throw in `PonyPolygons.cpp` is an untriggered tripwire, exactly per house philosophy. Don't re-raise.
8. ~~Mac toolchain pin drift~~ **Resolved 2026-07-13**: the `llvm@21` link workaround was verified obsolete on the Mac and removed entirely (commit `554acd68`), so there is no longer a compile/link version split to drift.
9. **Walk the regenerated code-coverage report** (`coverage_reports/TestAll`, regenerated 2026-07-13, commit `b0581ae5`): the user explicitly asked to add this to the future list — review what the report says about untested regions (cross-reference the known uncovered pure surfaces in Observations below) when they're ready, not before.

## Observations (no action urged; context for future work)

- `framebuffer_object::bind(texture_unit)` binds the FBO and samples its own colour attachment in one call — feedback-loop territory if rendered in that state.
- `unique_image::operator==`: deep equality for vector-backed, pointer identity for stb-backed.
- `polygon`'s EBO capture relies on the VAO staying bound across the construction tail (member-order coupling).
- Capability-gated test suites reduce to zero checks on the Mac floor with no positive "skipped" assertion.
- Untested pure surfaces: `Version.hpp` predicates, `to_gl_underlying_value`, `to_array`, N≥5 polygons.
- shaderc/volk submodules unused (staged for Vulkan lectures?); README dependency list omits them; no CI anywhere; Windows builds `cxx_std_23` vs 26 elsewhere.
- Era-stale expected-output files `ShaderProgramBrokenStagesFreeTest_Exceptions_Linux_Mesa_OpenGL_4_5.txt` (pre-restructure path `Graphics/OpenGL/`, old message format) and `..._Linux_NVIDIA_OpenGL_4_6.txt` (old message format, line 184): user said **leave for now** (2026-07-13) — neither retire nor regenerate; don't re-raise as stale.

**How to apply:** when the user says "let's return to the review findings" (or similar), start here, re-verify line numbers against current code, and check the Resolved section before raising anything. Individual items may also resolve naturally via lectures (L50 unification, the zstring_view discussion) — prune as they do.
