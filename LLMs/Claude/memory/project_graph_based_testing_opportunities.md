---
name: avocet-graph-based-testing-opportunities
description: Look for opportunities to apply graph-based (transition_checker) testing in avocet — current usage, surveyed candidates, review register
metadata:
  type: project
---

The user asked (2026-07-17) that I actively look for places where graph-based testing ([[graph-based-testing-recipe]]) would strengthen avocet's tests. It is the user's signature technique ([[sequoia-graph-based-testing-conviction]]) and they consider its expressiveness a shared context-compression win.

**Current usage (surveyed 2026-07-17): exactly one site** — `Tests/OpenGL/Capabilities/CapabilityManagerFreeTest.cpp`, which also establishes the hidden-state bridge pattern: `T` = pure payload value, each transition re-forces the GL context to the parent state, and the CheckFn verifies both the payload (equality) and the live context (weak_equivalence). Reviewed in the 2026-07-17 addendum to `LLMs/Claude/reviews/sequoia_review_2026-07-16.md` (empty edge descriptions; hand-sampled asymmetric transition matrix).

**Candidates spotted in the current suite** (raise as questions, next-lecture-sized; the user decides):

1. **Shader-program uniform state** (`ShaderProgramFreeTest.cpp:80-129`): a linear imperative walk alternating `set_uniform` between two programs — exactly the shape a transition graph replaces. Nodes = uniform-state payloads per program (predictions via `glGetUniform*`), edges = `set_uniform` calls; the two-program interleaving that implicitly exercises the program-binding cache becomes explicit cross-edges. Especially valuable *before* the planned DSA `glProgramUniform*` migration ([[project-shader-program-dsa-uniforms]]) — the graph pins behaviour across it.
2. **Binding cache × resource lifetime** (`ResourceTrackingUtilities.hpp` + Framebuffer/ShaderProgram tracking tests): today four hand-enumerated scenario methods (serial/threaded × overlapping/non-overlapping lifetimes). The serial ones are a state machine — nodes = (live resources, cache contents), edges = create/use/destroy — and a graph would make scenario coverage declarative and extensible (e.g. destroy-while-bound, recreate-after-destroy). Threaded variants stay outside the technique.
3. **Buffer/texture object state** (`BufferObjectTest`, `Texture2dTest`): bind/upload/resize sequences are transition-shaped; weaker candidates until those objects grow more mutating operations.

**Register:** these are opportunities, not obligations — some of the imperative tests may be pedagogically staged for lectures; flag as "worth a graph?" questions. When writing one, populate the edge descriptions (the CapabilityManager gap) and remember tests are real API customers ([[feedback-tests-are-customers]]).
