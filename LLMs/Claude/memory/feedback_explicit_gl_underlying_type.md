---
name: feedback-explicit-gl-underlying-type
description: Spelling the underlying type at each `to_gl_underlying_value<T>` call is deliberate redundancy — it lets a reader check the C++/OpenGL boundary against the GL spec's parameter types.
metadata:
  type: feedback
---

`to_gl_underlying_value<UnderlyingType>(Enum e)` (in `Source/avocet/OpenGL/Utilities/Casts.hpp`, introduced lecture 48) is constrained by `same_as<UnderlyingType, std::underlying_type_t<Enum>>`, so the explicit `<GLenum>` / `<GLint>` / `<GLboolean>` template argument is **type-redundant** — it can only ever be one value. That redundancy is **intentional and load-bearing**, not noise to be deduced away.

**Why:** the function is meant for use *only at the C++/OpenGL boundary*. Forcing the caller to spell the underlying type makes it trivial to eyeball each argument against the OpenGL spec's parameter list and confirm there's no unexpected conversion. The canonical example is the `glTexImage2D` call at `Source/avocet/OpenGL/Resources/Textures.hpp:133` — a long positional argument list (`internalformat` is `GLint`, `width`/`height` are `GLsizei`, `format` and `type` are `GLenum`) where `to_gl_underlying_value<GLint>(...)`, `checked_conversion_to<GLsizei>(...)`, `to_gl_underlying_value<GLenum>(...)` line up one-to-one with the spec signature and can be checked by inspection.

**How to apply:** Do **not** suggest deducing the underlying type, dropping the explicit argument, or "reducing verbosity" — you'd be erasing a deliberate doc-against-the-spec check. This is the same family of intent as [[feedback-deducing-this-convention]] and the `opt_out`/`driver_dependent` self-documentation types: spelling-as-documentation. Contrast with `to_underlying_value` (now `std::to_underlying`, see below) used *away* from the GL boundary, where no such explicit spelling is wanted. (Lecture 48 also switched the internal helper to `std::to_underlying`.)
