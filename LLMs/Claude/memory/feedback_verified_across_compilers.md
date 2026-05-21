---
name: feedback-verified-across-compilers
description: Don't raise speculative compiler-warning concerns in this repo — the user builds on MSVC, gcc, and clang and has already verified.
metadata:
  type: feedback
---

When reviewing code in HelloRenderingEngine, don't raise concerns of the form "this attribute/construct *might* warn on some compiler" or "a future toolchain *could* flag this" unless I have concrete evidence the warning actually fires.

**Why:** The user cross-builds on MSVC, gcc, and clang as a matter of course; if all three accept the code silently, the design is intentional and verified. I once flagged `[[nodiscard]]` on `gl_function` as potentially confusing for `void`-returning `operator()` calls — but I had misread the semantics (class-level `[[nodiscard]]` applies to discarding values of the class type, not to member-function returns), and no compiler had ever objected. The user (rightly) pushed back.

**How to apply:** Reserve attribute/warning critiques for cases where I can name the specific compiler and the specific diagnostic, or where the language rule is unambiguous. "Generic-code, three compilers, no complaints" is strong evidence the construct is correct as-is.
