---
name: feedback-check-production-sites
description: When evaluating whether a multi-valued enum/type earns its keep, check production sites, not just consumption sites.
metadata:
  type: feedback
---

When critiquing a multi-valued type (e.g. a tri-state enum) in this codebase, don't conclude "this carries no real information" purely from the fact that current consumers don't branch on every value. Check where the value is *produced* — the distinction may be genuine at construction even if downstream code groups some states together.

**Why:** I flagged `object_labelling_available` (no / yes / driver_dependent) in `Source/avocet/OpenGL/Context/ContextBase.hpp:59-63` as a "tri-state wearing a two-valued check" because all consumers (`add_label`, `extract_label`, etc.) only test `!= no`. But the producer at `ContextBase.cpp:18-22` (`labelling_available`) does distinguish all three — `yes` means debug output is on so label issues will surface, `driver_dependent` means labels are version-supported but the driver may silently drop them. The user pushed back on my framing.

**How to apply:** Before saying a type-level distinction is redundant, grep for both the construction sites and the comparison sites. If construction encodes information that consumers happen not to branch on yet, the right framing is "this state is informational/diagnostic today" — not "the type is doing nothing." Honour the producer's intent.

The user also flagged a broader principle in this codebase: types serve as durable documentation for *future readers* (especially the user returning to their own code months later), not only as control-flow machinery. A tri-state that no consumer currently branches on can still earn its keep purely by telling a future reader "this is the state of the world here" without making them re-derive it. Weigh this when judging whether finer-grained types or enum values are over-engineered — in this codebase, the answer often is "no, it's deliberate self-documentation."
