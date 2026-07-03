---
name: feedback-opengl-source-of-truth-in-tests
description: "In resource tests, the user recovers handles by querying OpenGL binding-point state, not by reading them off his own objects — OpenGL is the source of truth."
metadata: 
  node_type: memory
  type: feedback
  originSessionId: 972874bf-746a-4860-a5b6-ea430800f8e2
---

When verifying GPU-resource destruction (e.g. `check_gpu_cleanup` in `GraphicsTestCore.hpp`, added ~lecture 49), the user deliberately recovers the resource handle by *querying OpenGL binding-point state* (`glGet` via the `int_names` getters — `array_buffer_binding`, `texture_binding_2d`, `current_program`, `draw_framebuffer_binding`) rather than reading it off his own resource object. Two reasons he gave:

1. **OpenGL is the source of truth.** Deriving the handle from a potentially-buggy abstraction would let a bug in the object-under-test mask itself; querying GL independently keeps the test honest.
2. **Don't leak raw handles through the abstraction.** In a good design, clients shouldn't need the raw `GLuint`, so he avoids exposing `get_index`-style accessors as public API surface just to serve a test.

**Why:** this looks indirect/state-coupled at first glance (and I flagged it as a question), but it's a principled choice, not convenience. **How to apply:** don't suggest "just read the handle off the object with `get_index`" as a simplification in tests — the binding-point query is intentional. A consequence: such checks are coupled to bound/current state at query time (e.g. a shader program's handle is captured only after something `use()`d it, since the ctor doesn't). See [[feedback-tests-are-customers]].
