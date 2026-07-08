---
name: project-planned-lecture-topics
description: "Many of the \"deeper improvements\" I identified in a code review are already planned lecture topics — treat as active/roadmap work, not fresh suggestions."
metadata: 
  node_type: memory
  type: project
  originSessionId: 972874bf-746a-4860-a5b6-ea430800f8e2
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

## Terminology for the resource-hierarchy sub-series (as of 2026-06-30, provisional)

The user settled on a three-word scheme for the 5-lecture arc that harmonizes the "standard" GPU resources (buffers/VAOs/textures — batch `generate`/`destroy`/`bind`/`configure` life-events) with the program-related resources (`shader_program_resource_lifecycle` — singular `create`/`destroy`/`use`, no `configurator`):

- **Harmonization** — umbrella term for the whole sub-series (the goal/spirit: making the families consistent).
- **Canonicalization** — Lecture 49: establish the common form and draw the life-events types toward it (= *conform*; types come to satisfy a common concept). May unify one program-related resource as a proof of concept, but deliberately does **not** finish the job.
- **Unification** — Lecture 50 (next): *merge* — collapse the duplicate machinery, e.g. fold `generic_shader_resource` into `generic_resource`, once both sides speak the canonical form.

The load-bearing distinction: **canonicalization = conform, unification = merge**; conforming is a precondition that unification consumes (you can't merge until they conform).

Discussed (and dismissed) "normalization" as the L49 word — in CS it carries the same value-transform connotation as "canonicalize" (DB/Unicode/vector normalization), so it doesn't escape the concern. **Why:** the user agonizes over naming and asked me to frame these lectures consistently. **How to apply:** use these words as above when discussing the arc, but treat as provisional — the user explicitly flagged he may revise. Confirm before relying on it if it's been a while.

## The L49 canonical concept: `sequoia::pseudoregular` (confirmed 2026-07-03)

The concrete "common form" L49 will demand of every life-events type is **`sequoia::pseudoregular`** — like `std::regular` but with the default-constructibility requirement *relaxed* (still needs copyable + `operator==`). This is why the `start_lecture_49-preliminary` groundwork adds defaulted `constexpr`/`noexcept` `operator==` to *every* `*_lifecycle_events` type (vao/buffer/texture/framebuffer/shader-stage/shader-program) — none was consumed by a concept yet at the preliminary stage; it's setup for the pseudoregular constraint.

Relaxing default-constructibility is load-bearing, not incidental: `shader_stage_lifecycle_events` holds a `shader_species m_Species` and is therefore **not** default-constructible, so plain `std::regular` would exclude it. `pseudoregular` is chosen precisely so the stateful shader-stage type can conform alongside the empty tag-like types. **How to apply:** when reviewing L49 work, expect a `pseudoregular` requirement on the life-events concept(s); don't flag the `operator==` additions as unused, and don't suggest `std::regular` (it would wrongly reject the shader-stage type).

## Life-events member functions: static-or-member is by design (confirmed 2026-07-08, re L49→L50)

The life-events concepts (`has_configure_event_v`, `has_bind_event_v`, `has_use_event_v`, `standard_lifecycle_for`) are written to invoke on a **const instance** (`lifeEvents.configure(...)`, not `LifeEvents::configure(...)`) — the L49 "consistently relax the restriction on life events being carried by static functions" commit. The resulting mix is **intentional and will persist into L50**: a life-events type *may* carry state and use non-static (deducing-this) members, but if it has no state it's free to leave the member `static`. So across families you see `configure`/`bind` as `static` on the empty types (buffers/textures/framebuffer/`shader_program_lifecycle_events`) and as deducing-this members on the stateful `shader_stage_lifecycle_events` (holds `m_Species`). **How to apply:** do **not** flag this static-vs-member heterogeneity as an inconsistency to unify — the concept deliberately accepts both, and L50 keeps that latitude. See [[feedback-deducing-this-convention]] (the non-static `this const T&` there marks GPU/cache side-effects; here it's simply "carry state if you need it").

Other L49 confirmations (2026-07-08): per-shader-stage object labels are a *deliberate* nice side-effect of routing stage creation through `configure`+`add_label` (stages were previously unlabelled — only the linked program was). The eager label-string construction even when labels are unavailable (e.g. Mac floor) is known/accepted fallout, not an oversight — the user may revisit it only if a production build needs the optimization. The two dropped `static_assert(has_shader_lifecycle_events_v<...>)` lines were removed intentionally (they become fully redundant in L50).
