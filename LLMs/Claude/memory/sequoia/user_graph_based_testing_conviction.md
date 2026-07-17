---
name: sequoia-graph-based-testing-conviction
description: User's personal conviction — graph-based testing was load-bearing for sequoia's quality; held with explicit epistemic humility
metadata:
  type: user
---

The user's stated view (2026-07-17): graph-based testing (`StateTransitionUtilities.hpp`, slated for rename) enabled a rigour they couldn't previously achieve, and they don't believe sequoia could have reached its current state without the technique. Explicitly offered as personal opinion — "it's conceivable it doesn't stand up to scrutiny."

**Why:** this is the user's own assessment of their signature technique, held with deliberate humility — they *want* it stress-tested, not flattered.

**How to apply:** treat graph-based testing as the load-bearing pillar of sequoia's test strategy when reasoning about test architecture, but engage critically when evidence bears on the conviction either way. Corroborating evidence from the [[sequoia-review-2026-07]] review: the notoriously hard Connectivity bookkeeping held up under hostile review, and it is tested via this technique. Known limits (my scrutiny, shared with the user): coverage is bounded by declared states/edges — the loop-edge traversal bug escaped precisely because no declared graph contained a loop; the technique is exhaustive-example-based, not generative — pairing declared graph structure with generated node values would be the natural hardening; compositionality of per-edge checks relies on value semantics or on hidden state being independently verified per edge (as avocet's capability test does with the GL context).
