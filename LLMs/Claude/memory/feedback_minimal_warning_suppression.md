---
name: feedback-minimal-warning-suppression
description: The user suppresses driver warnings only on demonstrated need, per-config — unexpected warnings should be loud. Don't propose pre-emptive/unified suppression.
metadata:
  type: feedback
---

The user's policy on OpenGL driver warnings (and warning suppression generally): **suppress only when absolutely needed, at the config that needs it, when the need is demonstrated**. Unexpected warnings should surface loudly (as test failures via the throwing error-checker epilogue) so they can be triaged and then suppressed *knowingly*.

**Why:** In the 2026-07 review I flagged that `make_call_logging_window_config` (curlew) omits the per-renderer `ignored_warnings` list that `make_default_config` threads through, framing it as drift "one driver update from unrelated red tests." The user's verdict (2026-07-14): the asymmetry is the policy working. The call-logging tests' resource types (programs/framebuffers) have never triggered NVIDIA's 131185 buffer-object warning, so it isn't suppressed there; if a driver update or a new cached resource type ever fires it, the red run is *desired* — it's information, and the suppression gets added at that moment with full knowledge.

**How to apply:** Don't propose unifying/centralizing suppression lists across window configs, pre-emptively suppressing warnings a config hasn't hit, or "future-proofing" against driver chattiness. A missing suppression is presumed deliberate unless a test is actually failing. When a warning *does* newly fire, the right move is the minimal, per-config, commented addition — mirroring how `ignored_warnings`/`printed_then_ignored_warnings` (GLFWWrappers.cpp) are already per-renderer and enumerated individually. Same family as [[feedback-defensive-throws]] (loudness is a feature) and [[project-binding-cache-rationale]] (opt-in on demonstrated need, asymmetry as visible rationale).
