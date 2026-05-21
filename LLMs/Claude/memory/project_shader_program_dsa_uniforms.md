---
name: project-shader-program-dsa-uniforms
description: Acknowledged improvement — switch shader_program uniform setters from UseProgram+Uniform* to glProgramUniform* (DSA, core in 4.1).
metadata:
  type: project
---

`shader_program::set_uniform` / `get_uniform` (in `Source/avocet/OpenGL/Resources/ShaderProgram.hpp`) currently call `use()` before every `Uniform*` / `GetUniform*` call. The program-binding cache in `resourceful_context` makes the no-op case cheap, but using the DSA forms (`glProgramUniform*` / `glGetProgramUniform*`) would let the bind be skipped entirely.

**Why:** Raised this in a code-review pass; user agreed it was a good observation they hadn't considered. DSA uniform setters have been core since OpenGL 4.1, which is this project's minimum (see `opengl_version` default `{4, 1}` in `Source/avocet/OpenGL/Context/Version.hpp`). In a follow-up the user confirmed the switch *is* planned ("soon I will switch to glProgramUniform"), with the honest caveat that even knowing about it earlier they might have kept the bind-based path through the lectures to preserve pedagogical continuity — the program-binding cache itself was deliberate groundwork for an FBO driver warning (see [[project-binding-cache-rationale]]), and `shader_program::set_uniform` calling `use()` redundantly was the motivating example for introducing the cache in the first place.

**How to apply:** If asked to refactor or extend the uniform-setting API, prefer the DSA path. If reviewing this code again, don't re-raise the observation as if it were fresh — it's already acknowledged and on the way. When the switch happens, the per-program uniform cache (`m_Uniforms`) still earns its keep (location lookups are still per-program), but the program-binding cache's role narrows since `set_uniform` no longer touches it.
