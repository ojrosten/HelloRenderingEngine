---
name: feedback-defensive-throws
description: Defensive throws in this codebase are tripwires for invariants the user has hit during development — don't flag them as smells without context.
metadata:
  type: feedback
---

When reviewing this codebase, don't flag a `throw` that says "this should never happen" as a sloppy error path. The user adds these deliberately as tripwires for inconsistent states they have personally encountered during earlier development, where the diagnostic message saved real debugging time.

**Why:** I raised the throw in `init_debug` (`Source/avocet/OpenGL/Context/ContextBase.cpp:87`, fires when `debug_output_enabled()` is true but `GL_CONTEXT_FLAG_DEBUG_BIT` isn't set) as a constructor-throw from deep in the hierarchy that could surprise callers. The user clarified it's there because earlier development versions could get into exactly that inconsistent state, and the message was valuable. They acknowledge an `assert` might be philosophically purer (the invariant truly shouldn't reach this code in a correctly-set-up curlew window), but the throw earns its keep by remaining loud in release builds if a future change regresses.

**How to apply:** A defensive `throw std::runtime_error{...}` in this codebase — especially one whose message names the inputs that made it fire — is almost always intentional. If I want to raise it, the question to ask is not "is this throw justified?" but "is the diagnostic message specific enough?" or "would migrating to `assert` be appropriate given the invariant is now reliably maintained?" — and even then, only if I have a concrete reason to prefer the change.
