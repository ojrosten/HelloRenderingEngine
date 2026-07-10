---
name: feedback-deducing-this-convention
description: Non-templated `this const T& self` is a deliberate convention marking "logically const on the C++ object, but may have GPU side effects or mutate caching state".
metadata:
  type: feedback
---

The user has invented (possibly originated) a convention in this codebase using C++23 deducing-this *spelling* to encode a semantic distinction that plain `const` can't express. There are three signal levels:

| Spelling                                  | Meaning                                                                                                  |
|-------------------------------------------|----------------------------------------------------------------------------------------------------------|
| plain `void foo() const`                  | Object-const **and** doesn't mutate GPU state. GPU *queries* (`glGet*`, `glGetObjectLabel`, `glGetShaderiv`, `glGetShaderInfoLog`) qualify — they read, they don't change the world. |
| `void foo(this const T& self)`            | Object-const, but **may mutate GPU state** and/or mutate `mutable` cache state whose sole purpose is to suppress redundant GPU mutations. |
| `void foo(this const Self& self)` (template) | Same semantic intent as above, layered onto deducing-this used as the CRTP-free static-polymorphism mechanism (e.g. `polygon_base::draw` → derived `do_draw`). |

**The critical distinction is mutation, not "any GL call".** Calls that read GPU state (`glGet*`) are observably pure from the caller's perspective and are correctly plain `const`. Examples that I initially mis-flagged but are correctly `const`: `generic_resource::extract_label` (`glGetObjectLabel`), `shader_checker::get_parameter_value` / `get_info_log` (`glGetShaderiv`/`glGetShaderInfoLog`).

The motivation: the user reads `const` as signifying "no externally observable mutation" and is reluctant to mark GPU-mutating methods that way, even though they don't mutate the C++ object's value. The deducing-this form gives a *visually distinct* spelling for "object-const, GPU/cache may change."

Examples consistent with the convention:
- `context_base` getters (`debug_mode`, `glad_context`, `fundamental_characteristics`) are plain `const` — pure.
- `context_base::invoke` / `decorated_context::invoke` are deducing-this — they fire arbitrary GL calls (potentially mutating).
- `resourceful_context::utilize` is deducing-this — may write through to the `mutable` `m_Cache` and fire mutating GL calls.
- `gl_function::operator()`, resource `bind`/`use` — deducing-this, GPU-state mutating.
- `generic_buffer_object::extract_data`, `generic_texture_2d::extract_data`, `framebuffer_object::extract_data` — deducing-this. (TODO resolved 2026-07-10: these are *correctly* deducing-this. Although the `glGet*` reads are pure, each `extract_data` first **binds** the resource (`do_utilize()` — required for the non-DSA query forms), and binding mutates GL binding-point state. So the convention's mutation criterion is met via the bind, not the read.)

**Known exception to fix:** `shader_program::set_uniform` (and `do_set_uniform`) is currently plain non-const but *does* mutate GPU uniform state. By the convention it should be deducing-this on a const self. The fix is naturally bundled with the planned migration to `glProgramUniform*` — see [[project-shader-program-dsa-uniforms]] — because under DSA the "must be currently bound" coupling goes away and set_uniform becomes straightforwardly a GPU-state mutation on *this* program.

**Why:** The user explicitly said: *"I don't like marking functions const since I take that to signify no side effects. The explicit (const) object parameter (non-templated) seems to me a nice convention to convey what I'm looking to express."* GPU calls are externally observable; cache mutation breaks bitwise-const expectations even when the object's value is unchanged.

**How to apply:**

- **Reading the code:** When you see non-templated `this const T& self`, expect possible GPU effects and/or `mutable` cache mutation. Plain `const` is a stronger guarantee here than the language alone requires.
- **Suggesting changes:** Don't propose collapsing deducing-this onto plain `const` for "uniformity" or "less typing" — you'd be erasing a signal the user is deliberately maintaining. Conversely, if you find a method that fires GL calls or touches `m_Cache` but is spelled plain `const`, that's a *real* observation worth flagging (it breaks the convention).
- **Writing new methods:** Match the spelling to the semantics. New GPU-touching method on a context/resource → deducing-this. New pure getter → plain `const`.
- **Don't conflate with [[user-teaching-project]]'s "may be pedagogical"**: this isn't a feature demo, it's a working convention used as the type-system-adjacent vocabulary of the codebase. The pedagogical project [[project-binding-cache-rationale]] (`mutable` cache) is exactly the case this convention is built to flag.

Related: the templated `this const Self&` form (Pattern B in the conversation where this was clarified) does double duty — same side-effect semantics *plus* CRTP-free static dispatch. See `polygon_base::draw`, `shader_checker::check`, `common_texture_2d_lifecycle_events::configure`.
