---
name: project-binding-cache-rationale
description: The resourceful_context binding cache exists to silence a real FBO driver warning, with shader-program caching as deliberate pedagogical warm-up. Opt-in by design.
metadata:
  type: project
---

The `index_cache<caching_identifier>` mechanism in `Source/avocet/OpenGL/StateAwareContext/ResourcefulContext.hpp` (currently caches only `caching_identifier::program` and `caching_identifier::framebuffer`) is **opt-in by design**, not partial-by-omission. Three things to know:

1. **The load-bearing motivation is an FBO driver warning** the user knew was coming and couldn't sensibly ignore. The framebuffer cache is the real customer; the program cache predates it and was developed as deliberate pedagogical groundwork (lectures showed an evolution of solutions, starting from redundant `glUseProgram` calls in `shader_program`, so the machinery existed before the FBO problem arrived).

2. **`caching_identifier::opt_out` is intentional self-documentation.** It explicitly marks "I considered caching this resource and chose not to" — distinct from "I forgot." Same principle as `object_labelling_available::driver_dependent` (see [[feedback-check-production-sites]]): types here also serve as durable documentation for future-readers. The opt-out marker on VAOs/buffers/textures/etc. is *not* a TODO or omission to be cleaned up.

3. **New resources opt in only when there's a real need.** Textures will eventually want it (per the deeper-review discussion), but adding it pre-emptively would obscure the rationale. The asymmetry between program/framebuffer (cached) and everything else (opt_out) is the rationale being made visible.

**How to apply:** Don't suggest "cache everything" or "remove the cache" as a clean-up. If a future resource type starts hitting redundant-bind warnings or measurable cost, the answer is to add a new `caching_identifier` enumerator and extend `tuple_t` — not to rethink the architecture. The framework is designed to be grown incrementally.
