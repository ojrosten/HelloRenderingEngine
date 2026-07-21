# CLAUDE.md

Project-wide context for Claude Code (and any LLM coding assistant) working in this repository. Loaded automatically into every session.

This file holds the durable project-knowledge — orientation, conventions, and architectural rationale that a fresh assistant should know on turn one. Personal-collaboration notes and finer-grained context live as per-session memory in `LLMs/Claude/memory/` (see `LLMs/Claude/README.md`).

## What this project is

HelloRenderingEngine is a **teaching project** with accompanying ~1-hour lectures (51 delivered as of 2026-07-21). Each lecture is roughly 40 minutes presentation + 20 minutes live coding, with some infrastructure prepared off-line. The course is on **practical software engineering**, taught against a modern-C++ + OpenGL backdrop — cross-platform compromise, fallback design, capability gating, driver-bug workarounds, dependency management, and test infrastructure are *content*, not obstacles.

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

**OpenGL floor is 4.1.** The `opengl_version{4, 1}` default in `Source/avocet/OpenGL/Context/Version.hpp` is the Mac floor, not arbitrary. Features above the floor (DSA `glCreate*`/`glNamedBuffer*` at 4.5, `glDebugMessageCallback`/`glObjectLabel` at 4.3, `glBufferStorage` at 4.4) are fine to suggest — fallback design is the *engineering content* of such suggestions, not a tax on them. The existing codebase models this carefully (`debug_output_supported`/`object_labels_supported` in `Version.hpp`, the `#ifdef __cpp_lib_generator` guard in `Errors.hpp`).

**Standard library floor is libc++.** Notably `std::generator` is not yet available on libc++ — hence the `#ifdef __cpp_lib_generator` guard (which selects `std::generator` where present and the `libcpp_workaround` namespace's vector-returning fallback otherwise) in `Source/avocet/OpenGL/Debugging/Errors.cpp`. The Mac build uses Homebrew llvm's clang via the CMake presets; a former `target_link_directories` workaround forcing the link against Homebrew's libc++ became unnecessary and was removed in July 2026 (visible on older lecture branches).

## Conventions

### Lecture branch naming

While developing a lecture's code, branches use a three-stage refinement suffix:

1. `end_lecture_X-experimental` — earliest draft. Structure may still change. Sub-optimal code is likely work-in-progress; feedback can be broad.
2. `end_lecture_X-preliminary` — direction settled; details in flux. Focus on what's missing or uncomfortable, not rearchitecting.
3. `end_lecture_X-expected` — close to what will be presented. Tight, high-confidence feedback only.

Once delivered, the branch lands as `end_lecture_X` (no suffix) and is the stable per-lecture anchor. When reviewing a `-suffix` branch, diff against the prior lecture's stable branch (`end_lecture_<X-1>`).

Each live lecture *begins* from `start_lecture_X` — the offline-prepared state from which the live coding proceeds to `end_lecture_X`. Prep for these follows the same suffix pattern (heavy use of `start_lecture_X-preliminary`, merged into `start_lecture_X` once the material is settled), and may iterate during rehearsal: more pre-prepared material added, or — when there's time to do more live — material *removed* from the start branch. `start_lecture_X` is therefore mutable until delivery; `start_lecture_X..end_lecture_X` is the lecture's on-stage content.

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

### Session-start memory-sync check

The version-controlled `LLMs/Claude/memory/` (source of truth) and the per-machine auto-memory directory (the path is given in your system prompt) can drift — memory writes during a session land in the auto-memory copy, while edits to the committed files land in the repo. **At the start of a session, quietly compare the two directories** (e.g. `diff -rq LLMs/Claude/memory "<auto-memory-path>"`) and, if they differ, tell the user which side is ahead and offer to reconcile. Don't sync automatically — the user decides direction:
- **repo → auto-memory:** run `./LLMs/Claude/sync.sh "<auto-memory-path>"` (this is the normal direction; repo is canonical).
- **auto-memory → repo:** copy the changed `*.md` back into `LLMs/Claude/memory/`, then it shows in `git diff` to review and commit.

Keep the check lightweight and silent when the two are already in sync.

### Session-start lecture-count check

The delivered-lecture count N is recorded in **two places**: the "(N delivered as of ...)" parenthetical at the top of this file, and the "Lecture format & cadence" line in `LLMs/Claude/memory/user_teaching_project.md`. At the start of a session, check the current branch (it's in the session's git snapshot):

- If it is **exactly** `end_lecture_X` (no suffix) and X > N: lecture X has been delivered (unsuffixed branches only land post-delivery). Update N to X in both places — use the branch tip's commit date as the "as of" date — mention it, and re-run the memory sync so the auto-memory copy follows. Lectures generally come with **homework** — a small tweak deliberately left for attendees — but it's only sometimes recorded in the commit message. Check the lecture's commit messages (`git log end_lecture_<X-1>..end_lecture_X`); if no homework is mentioned, ask the user whether lecture X had any, and record the answer in the homework memory (`project_lecture_homework.md`).
- If the branch carries a refinement suffix (`end_lecture_X-experimental`/`-preliminary`/`-expected`, or any `start_lecture_X` form): do **not** update — the lecture hasn't been delivered yet.
- If X ≤ N or the branch isn't lecture-shaped: nothing to do; stay silent.
