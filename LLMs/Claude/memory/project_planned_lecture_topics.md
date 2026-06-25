---
name: project-planned-lecture-topics
description: Many of the "deeper improvements" I identified in a code review are already planned lecture topics — treat as active/roadmap work, not fresh suggestions.
metadata:
  type: project
---

When I delivered a "deeper improvements" review of the codebase on 2026-05-21, the user confirmed that *many* (not all enumerated) of my structural observations are already planned topics for future lectures in the teaching series. The areas I raised that fall into this category include:

- Strongly-typed texture parameters (replacing the `parameter_setter` `std::function` escape hatch)
- Buffer storage policies beyond `GL_STATIC_DRAW` / `glBufferData` (dynamic/streaming, `glBufferStorage` with persistent-mapping)
- Render-graph / draw-queue abstraction (FBO push/pop, sortable draw queue, render passes)
- UBO / shared-uniform abstraction
- ~~Rationalising the partial binding cache in `resourceful_context`~~ — NOT planned; the partial cache is intentional opt-in design (see [[project-binding-cache-rationale]]). The DSA-uniforms switch ([[project-shader-program-dsa-uniforms]]) *is* coming and will narrow the program cache's role.
- Unifying `generic_resource` and `generic_shader_resource` (this one specifically is the **endpoint of a current 5-lecture sub-series** — i.e. live, not future roadmap)
- FBO depth/stencil attachment support
- **Arithmetic promotions / `std::in_range` / `std::cmp_*`** — provisionally **lecture 51** (as of lecture 48, delivered ~2026-06; the user notes it may slip later, since topics often take more lectures than estimated). This is why `checked_conversion_to` (lecture 48) hand-rolls its bounds checks instead of using `std::in_range`: the standard facilities are being saved for this dedicated treatment. Do **not** suggest switching `checked_conversion_to` to `std::in_range`/`std::cmp_*` as an improvement — it's deliberate setup. See [[feedback-explicit-gl-underlying-type]] / the lecture-48 conversion utilities.

**Why:** Knowing these are on the teaching roadmap (and one is actively in flight) changes how I should respond when they come up. Re-raising them as if they were fresh observations would be tedious and miss the user's actual context.

**How to apply:** If the user works on any of these areas, treat it as the planned lecture work rather than introducing it as a suggestion. If asked to help with the resource-hierarchy unification specifically, recognise it's the current sub-series and the user has been thinking about it carefully for ~5 lectures' worth of context — ask what stage they're at before offering structural alternatives. Don't enumerate the same "improvements" list again unprompted; the user already knows what's coming.

Since this list isn't exhaustive (the user said "many," not "all"), there may still be deep observations that *are* novel — but lead with curiosity ("is this on the lecture roadmap?") before treating something as a fresh contribution.
