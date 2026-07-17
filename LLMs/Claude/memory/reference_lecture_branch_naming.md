---
name: reference-lecture-branch-naming
description: Lecture-development branches use end_lecture_X-{experimental,preliminary,expected} suffixes that mark refinement stage.
metadata:
  type: reference
---

While developing a new lecture's code, the user uses a three-stage branch naming convention. Order reflects increasing refinement:

1. **`end_lecture_X-experimental`** — earliest draft. Most rough. Ideas are still being tried out; structure is likely to change.
2. **`end_lecture_X-preliminary`** — refined draft. Major direction settled, details still in flux.
3. **`end_lecture_X-expected`** — most refined. Close to what will actually be presented in the lecture.

Once the lecture is delivered, the branch lands as `end_lecture_X` (no suffix), which is the stable per-lecture anchor (see [[user-teaching-project]]).

## start_lecture_X branches (confirmed 2026-07-10)

Each live lecture *begins* from **`start_lecture_X`** — the offline-prepared state (infrastructure the user readies before the lecture; the ~20 min of live coding then carries it toward `end_lecture_X`). During prep these follow a similar suffix pattern to `end_lecture_X`, with **heavy use of `start_lecture_X-preliminary`**: once confident in the material, the user merges it into `start_lecture_X`. This can iterate during rehearsal:

- If rehearsal shows more pre-prepared material is needed, more is added via another `-preliminary` → merge cycle.
- Occasionally the opposite: if there's time to do more live, material is **removed** from `start_lecture_X`/`-preliminary` — so content flowing *out* of the start branch is normal, not a regression.

So `start_lecture_X` is *mutable until delivery*, unlike the delivered `end_lecture_X` anchors.

**How to apply:** When reviewing or diffing one of these branches:

- **Diff base:** the prior lecture's stable branch is usually right — e.g. for `end_lecture_47-experimental` *or* `start_lecture_47-preliminary`, diff against `end_lecture_46`. If iterating *within* the same lecture (e.g. `-preliminary` vs `-experimental`), diff between those two to see what was tightened. For assessing the live-coding delta itself, `start_lecture_X..end_lecture_X` is the lecture's on-stage content.
- **Review register:** match the branch stage.
  - `-experimental`: structural ideas welcome, sub-optimal code is likely work-in-progress, feedback can be broad. Don't fixate on polish.
  - `-preliminary`: direction is settled — focus on "what's missing" or "what would I be uncomfortable presenting like this," not on rearchitecting.
  - `-expected`: tight, high-confidence feedback only. The user is close to recording/presenting; this is the wrong moment to introduce new ideas.

If the branch name doesn't carry a suffix, treat it as already-delivered stable content.