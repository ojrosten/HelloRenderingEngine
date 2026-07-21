---
name: project-vulkan-branch-merges
description: "Recipe for merging lecture branches into vulkan-experiment — rename propagation, verification via Demo run, Windows gotchas"
metadata: 
  node_type: memory
  type: project
  originSessionId: c54c52f7-5e6f-44df-8d82-eef0e7a8e9ba
---

The `vulkan-experiment` branch periodically receives merges from lecture branches (first done: start_lecture_51 → vulkan-experiment, 2026-07-17). The Vulkan code is experimental and not yet held to the project's usual standard.

**Why:** The branch renamed `curlew::window`/`window_config`/`rendering_setup` to `opengl_window`/`opengl_window_config`/`opengl_rendering_setup` to distinguish GL from Vulkan. Lecture code arriving via merge uses the old names and won't conflict in files the Vulkan branch never touched.

**How to apply:**
- Propagate the `opengl_*` renames into all incoming lecture code (including tests) — that's the branch's convention; don't add back-compat aliases.
- Acceptance test is the **Demo project running correctly** (GL + Vulkan windows); the test suite matters but there are no Vulkan tests yet.
- Running Demo.exe headlessly on Windows: put the MSVC toolset bin dir (for `clang_rt.asan_dynamic-x86_64.dll`) on PATH; let it render ~10s, then close its windows via `PostMessage WM_CLOSE` (PowerShell EnumWindows) so the render loop exits and stdout flushes — killing the process loses buffered output and can leave a zombie Demo.exe holding a lock on the exe (breaks the next link with LNK1104).
- After a Visual Studio update, `build/*/msvc-asan` CMake caches pin the old toolset path — wipe and reconfigure (VS may hold locks on `.vs/`; only the CMakeCache matters).
- Healthy Demo output on the user's machine (Intel Arc 140V): GL 4.6, Vulkan layer/extension listing, plus the printed-then-ignored Intel `message_id{2}` recompile warning.
