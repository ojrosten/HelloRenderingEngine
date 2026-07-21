---
name: memory-skill-promotion-check
description: When writing or substantially extending a procedural/recipe memory, judge whether it should be promoted to a proper skill
metadata:
  type: feedback
---

Standing practice agreed with the user (2026-07-20): some memories are procedural in spirit — recipes like [[graph-based-testing-recipe]] or [[project-coverage-report-generation]] — and do the *job* of a skill while being memories *mechanically*. Whenever I create or substantially extend such a memory, I should pause and judge whether it has outgrown memory-hood and deserves conversion to a proper skill (a `SKILL.md` under `.claude/skills/`, invoked by name).

**Why:** the two mechanisms load differently — memories are recalled by relevance (ambient, sometimes missed), skills are loaded whole on invocation (deliberate, complete). A long recipe wants to arrive complete and on demand; a calibration wants to surface unbidden. The user asked for this judgement to be made periodically; the event-driven trigger (touching the file) is the reliable form of "periodically" for me.

**Promotion signals:** step-by-step how-to; long or multi-section; used by deliberate invocation ("generate the coverage report") rather than ambient relevance; self-contained task with inputs/outputs. **Anti-signals:** calibration, preferences, project facts, anything whose value is surfacing *uninvited* during other work.

**How to apply:** on promotion, the skill carries the procedure; keep (or shrink to) a small memory pointing at the skill so ambient recall still routes there — don't lose the relevance-recall path entirely. Raise the suggestion to the user rather than converting unilaterally; skills live in the repo's `.claude/` and are their infrastructure.
