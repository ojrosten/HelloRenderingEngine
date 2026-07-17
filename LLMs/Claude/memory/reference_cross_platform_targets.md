---
name: reference-cross-platform-targets
description: Cross-platform support matrix — Windows/Linux/Mac × MSVC/gcc/clang × OpenGL 4.1 floor (Mac) to 4.6 ceiling (Win).
metadata:
  type: reference
---

HelloRenderingEngine must build and run on three platforms with three toolchains and three OpenGL ceilings:

| Platform | Compiler   | Std library  | OpenGL  |
|----------|------------|--------------|---------|
| Windows  | MSVC       | MSVC STL     | 4.6     |
| Linux    | gcc        | libstdc++    | 4.x (currently 4.5 on WSL, which the user uses) |
| macOS    | clang      | libc++       | 4.1 (Apple-deprecated, floor) |

The user personally develops on WSL most of the time, but Windows and macOS are first-class targets and CI/tests run across all three.

**OpenGL floor is 4.1.** That sets the bar for any API suggestion. Specifically:
- `glProgramUniform*` — core 4.1 ✓ portable
- `glDebugMessageCallback` / `glObjectLabel` — core 4.3 ✗ Mac (explains why `object_labels_available()` returns `no` on Mac and the debug subsystem is silently absent — see `Version.hpp:21-28`)
- `glBufferStorage` (immutable storage, persistent mapping) — core 4.4 ✗ Mac
- `glCreate*` (DSA creation: `glCreateBuffers`, `glCreateTextures`, `glCreateFramebuffers`, …) — core 4.5 ✗ Mac
- `glNamedBufferData` / `glTextureStorage*` / `glMapNamedBufferRange` — DSA, generally 4.5 ✗ Mac

The `opengl_version{4, 1}` default in `Source/avocet/OpenGL/Context/Version.hpp` is the Mac floor, not an arbitrary value.

**Standard library floor is libc++.** Specifically:
- `std::generator` is not yet available on libc++ — hence the `#ifdef __cpp_lib_generator` guards (in `Errors.hpp` and `Errors.cpp`) and the `libcpp_workaround` namespace in `Source/avocet/OpenGL/Debugging/Errors.cpp`. (The earlier `STD_GENERATOR` macro was removed when an MSVC update shipped `<generator>` — see [[feedback-lecture-diff-is-superset]]; updated 2026-07-10.)
- The Mac build compiles with Homebrew llvm's clang (pinned in the CMake presets). The former `target_link_directories(avocet PUBLIC /opt/homebrew/opt/llvm@21/lib/c++)` workaround in `Source/avocet/CMakeLists.txt` was verified obsolete on the Mac and removed 2026-07-13 (commit `554acd68`) — visible only on older lecture branches now.

**How to apply:** Don't avoid suggesting features that need OpenGL > 4.1 or library facilities not yet in libc++ — the user has explicitly said *not* to hold back. The course is on practical software engineering, and cross-platform constraints are exactly the kind of real-world problem the course addresses.

The right shape of a suggestion is:
1. Name the desired feature plainly (e.g. `glBufferStorage`, `std::generator`).
2. Identify which platform(s) it excludes (e.g. "4.4 — not on Mac; libc++ doesn't ship `std::generator` yet").
3. Sketch the fallback strategy as part of the suggestion: capability gating (cf. `debug_output_supported`/`object_labels_supported` in `Version.hpp:21-28`), runtime version check, macro-guarded alternate path (cf. `STD_GENERATOR` in `Errors.hpp:20-27`), or a typed `std::optional` capability handle.

In other words, present the fallback design as the *engineering content* of the suggestion, not as a tax on it. The user models this carefully in the existing codebase — match that style.