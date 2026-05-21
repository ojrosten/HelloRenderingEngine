# CLAUDE.md

Project-wide context for Claude Code (and any LLM coding assistant) working in this repository. Loaded automatically into every session.

This file holds the durable project-knowledge — orientation, conventions, and architectural rationale that a fresh assistant should know on turn one. Personal-collaboration notes and finer-grained context live as per-session memory in `LLMs/Claude/memory/` (see `LLMs/Claude/README.md`).

## What this project is

HelloRenderingEngine is a **teaching project** with accompanying ~1-hour lectures (~46 delivered so far). Each lecture is roughly 40 minutes presentation + 20 minutes live coding, with some infrastructure prepared off-line. The course is on **practical software engineering**, taught against a modern-C++ + OpenGL backdrop — cross-platform compromise, fallback design, capability gating, driver-bug workarounds, dependency management, and test infrastructure are *content*, not obstacles.

Consequences worth keeping in mind:

- The codebase grows incrementally in small, lecture-sized steps. The right register for suggestions is "next-lecture-sized," not "sweeping refactor across the engine."
- Some code is deliberately sub-optimal — usually because the user plans to refine it in a future lecture. But not always; the user makes genuine mistakes too. Flag observations as questions ("is this intentional groundwork or worth a fix now?") rather than pre-deciding either way.
- Some constructs exist primarily to *demonstrate* a modern C++ feature, not because they are production-optimal. The user addresses production trade-offs in the accompanying lecture rather than in the code itself.

## Cross-platform targets

The project must build and run on three platforms with three toolchains and three OpenGL ceilings:

| Platform | Compiler | Std library | OpenGL                                       |
|----------|----------|-------------|----------------------------------------------|
| Windows  | MSVC     | MSVC STL    | 4.6                                          |
| Linux    | gcc      | libstdc++   | 4.x (currently 4.5 on WSL, primary dev env) |
| macOS    | clang    | libc++      | 4.1 (Apple-deprecated, floor)               |

**OpenGL floor is 4.1.** The `opengl_version{4, 1}` default in `Source/avocet/OpenGL/Context/Version.hpp` is the Mac floor, not arbitrary. Features above the floor (DSA `glCreate*`/`glNamedBuffer*` at 4.5, `glDebugMessageCallback`/`glObjectLabel` at 4.3, `glBufferStorage` at 4.4) are fine to suggest — fallback design is the *engineering content* of such suggestions, not a tax on them. The existing codebase models this carefully (`debug_output_supported`/`object_labels_supported` in `Version.hpp`, the `STD_GENERATOR` macro in `Errors.hpp`).

**Standard library floor is libc++.** Notably `std::generator` is not yet available on libc++ — hence the `STD_GENERATOR` macro guard and `libcpp_workaround` namespace in `Source/avocet/OpenGL/Debugging/Errors.cpp`. The `target_link_directories(... /opt/homebrew/opt/llvm@21/lib/c++)` block in `Source/avocet/CMakeLists.txt` arranges the Mac build to link against Homebrew llvm@21's libc++, not Apple's.

## Conventions

### Lecture branch naming

While developing a lecture's code, branches use a three-stage refinement suffix:

1. `end_lecture_X-experimental` — earliest draft. Structure may still change. Sub-optimal code is likely work-in-progress; feedback can be broad.
2. `end_lecture_X-preliminary` — direction settled; details in flux. Focus on what's missing or uncomfortable, not rearchitecting.
3. `end_lecture_X-expected` — close to what will be presented. Tight, high-confidence feedback only.

Once delivered, the branch lands as `end_lecture_X` (no suffix) and is the stable per-lecture anchor. When reviewing a `-suffix` branch, diff against the prior lecture's stable branch (`end_lecture_<X-1>`).

### Deducing-this as side-effect marker (non-templated form)

C++23 deducing-this is used in two distinct ways in this codebase:

**Pattern A — non-templated `this const T& self`** is a *convention* (possibly invented here) for member functions that are logically const on the C++ object but *may mutate GPU state* and/or mutate `mutable` cache state whose sole purpose is to suppress redundant GPU mutations. The rule is about **mutation**, not "any GL call":

| Spelling                            | Meaning                                                                                          |
|-------------------------------------|--------------------------------------------------------------------------------------------------|
| `void foo() const`                  | Object-const **and** doesn't mutate GPU state. GPU *queries* (`glGet*`) are fine here.           |
| `void foo(this const T& self)`      | Object-const, but **may mutate GPU state** and/or the `mutable` cache.                           |
| `void foo(this const Self& self)`   | Pattern B — see below.                                                                           |

Plain `const` is reserved for stronger-than-language-required no-mutation guarantees. The deducing-this spelling carries the "world may change" signal that `const` alone would erase. Known exception (to be fixed alongside the planned DSA migration): `shader_program::set_uniform` is currently non-const but should be deducing-this — `glUniform*` mutates uniform state.

**Pattern B — templated `this const Self& self`** is genuine static dispatch — CRTP-free polymorphism. The base provides the algorithm; the derived class supplies hook methods/data; `Self` deduces to the most-derived type so `self.do_thing()` resolves without virtual dispatch. Examples: `polygon_base::draw` → derived `do_draw` (`Source/avocet/OpenGL/Geometry/Polygon.hpp`), `shader_checker::check` and the texture lifecycle-event traits.

### Defensive throws are intentional tripwires

`throw std::runtime_error{...}` calls with "should never happen" messages — e.g. `init_debug` in `Source/avocet/OpenGL/Context/ContextBase.cpp` — are deliberately placed. The user has hit those inconsistent states during earlier development; the diagnostic messages saved real debugging time. They earn their keep by remaining loud in release builds if a future change regresses. Don't flag them as sloppy error paths. If anything's worth raising, it's "would `assert` be philosophically purer here?" — and even then only with concrete cause.

## Architecture notes

### The binding cache is opt-in by design

The `index_cache<caching_identifier>` machinery in `Source/avocet/OpenGL/StateAwareContext/ResourcefulContext.hpp` caches only `caching_identifier::program` and `caching_identifier::framebuffer`. This is **opt-in**, not partial-by-omission:

1. The load-bearing motivation is an FBO driver warning that couldn't sensibly be ignored. The framebuffer cache is the real customer; the program cache predates it and was developed as deliberate pedagogical groundwork (lectures showed an evolution of solutions starting from redundant `glUseProgram`).
2. `caching_identifier::opt_out` on VAOs/buffers/textures/etc. is *self-documentation* — "I considered caching this and chose not to," distinct from "I forgot." Same principle as `object_labelling_available::driver_dependent` elsewhere: types here serve as durable documentation for future-readers.
3. New resources opt in only when there's measured need. Don't suggest "cache everything" or "remove the cache" as cleanup; the asymmetry *is* the rationale being made visible.

### Tests are real customers of the API

Tests in `Tests/` exercise parts of the API that may not have a current production caller. Before claiming any plumbing is unused (e.g. the `alignment` parameter of `unique_image`), grep `Tests/` — `AlignmentTest`, `PaddedImageSizeFreeTest`, and similar are genuine customers and the parameter is set to non-trivial values there.

## Known acknowledged TODOs

- **`shader_program::set_uniform` → `glProgramUniform*`** (DSA, core in 4.1). Currently `use()`s the program before each `Uniform*` call; the program-binding cache makes the no-op cheap, but DSA removes the bind entirely. Acknowledged and planned. Will narrow the program-binding cache's role and fix the deducing-this convention exception noted above.
- **C++26 reflection over `GladGLContext`** will eventually replace the hand-maintained `glad_ctx_member_info` table in `ContextBase.hpp`. OpenGL is a frozen API so the table is *not* a drift risk in the meantime — don't flag it as such.

## OpenGL is frozen

No new entry points will be added to OpenGL. Hand-maintained tables of GL functions can't silently fall out of sync the way a list of, say, REST endpoints could. Compile-time and runtime costs are fair game to mention; "fragile/maintenance burden" framing is not.

## More context

`LLMs/Claude/README.md` describes the layout of finer-grained per-session memory and how to make it visible on a fresh machine. The memory files in `LLMs/Claude/memory/` carry personal-collaboration calibration that isn't useful as always-loaded context but is recalled selectively by the auto-memory system once synced into place.
